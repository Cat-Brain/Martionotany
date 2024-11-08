#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "UI.h"
#include "Player.h"

#define TYPE(type, name) type name; \
Component(type&& name) : \
	name(name) { } \
operator type&() { return name; }

union Component
{
	TYPE(BaseComponent, base);
	TYPE(MeshRenderer, meshRenderer);
	TYPE(Position, position);
	TYPE(Scale, scale);
	TYPE(Rotation, rotation);
	TYPE(Camera, camera);
	TYPE(CameraMouse, cameraMouse);
	TYPE(FollowCursor, followCursor);
	TYPE(PhysicsBody, physicsBody);
	TYPE(PhysicsCircle, physicsCircle);
	TYPE(PhysicsBox, physicsBox);
	TYPE(InfinitePhysicsWall, infinitePhysicsWall);
	TYPE(Gravity, gravity);
	TYPE(MouseInteractable, mouseInteractable);
	TYPE(InteractableColor, interactableColor);
	TYPE(OnInteract, onInteract);
	TYPE(Player, player);

	constexpr Component& operator=(const Component& component)
	{
		return *this = component;
	}
};

class Entity
{
public:
	bool firstFrame;
	vector<Component> components;
	vector<tuple<System*, uint, uint>> systems; // [System, Outer Index (Which entity parameter this meets), Inner Index (Which entity is this)

	Entity(vector<Component> components, bool firstFrame = true) :
		firstFrame(firstFrame), components(components), systems{} { }

	bool HasRequirements(CompReq& requirements)
	{
		for (CHash antiTag : requirements.antiTags)
			for (Component& component : components)
				if (component.base.hash_code == antiTag)
					return false;
		for (CHash requirement : requirements.requirements)
		{
			bool invalid = true;
			for (Component& component : components)
				if (component.base.hash_code == requirement)
				{
					invalid = false;
					break;
				}
			if (invalid)
				return false;
		}
		for (CHash tag : requirements.tags)
		{
			bool invalid = true;
			for (Component& component : components)
				if (component.base.hash_code == tag)
				{
					invalid = false;
					break;
				}
			if (invalid)
				return false;
		}
		return true;
	}

	Component& GetComponent(CHash desired)
	{
		for (Component& component : components)
			if (component.base.hash_code == desired)
				return component;
		assert(false);
		return components[0];
	}
};

namespace ECS
{
	vector<Entity> entities{};

	int AddEntity(Entity&& entity)
	{
		uint entityId = static_cast<uint>(entities.size());
		entities.push_back(entity);
		for (System* system : System::systems)
			for (int i = 0; i < system->requirements.size(); i++)
				if (entity.HasRequirements(system->requirements[i]))
				{
					uint index = static_cast<uint>(system->entities[i].size());
					// Add entity to system:
					system->entities[i].push_back({ entityId, static_cast<uint>(entity.systems.size()),
						vector<ushort>(system->requirements[i].requirements.size())});

					for (int j = 0; j < system->requirements[i].requirements.size(); j++)
						for (uint k = 0; k < entity.components.size(); k++)
							if (entity.components[k].base.hash_code == system->requirements[i].requirements[j])
							{
								get<2>(system->entities[i][index])[j] = k;
								break;
							}

					// Save system and index in entity:
					entity.systems.push_back({ system, i, index });
				}
		return static_cast<int>(entities.size() - 1);
	}

	void RemoveEntity(uint index)
	{
		Entity& entity = entities[index];

		for (auto& [system, outerIndex, innerIndex] : entity.systems)
		{
			system->entities.erase(system->entities.begin() + innerIndex);
			for (int i = innerIndex; i < system->entities.size(); i++)
				get<2>(entities[get<0>(system->entities[outerIndex][i])].systems[get<1>(system->entities[outerIndex][i])])--;
		}
	}

	Entity& GetEntity(int index)
	{
		assert(index >= 0);
		return entities[index];
	}

	Entity& GetEntityReversed(int index)
	{
		assert(index >= 0);
		return entities[entities.size() - 1 - index];
	}

	size_t EntityCount()
	{
		return entities.size();
	}

	void Start()
	{
		for (GenericSystem* func : GenericSystem::startups)
			if (func->callRelativity == Before)
				func->fun();
		for (System* system : System::startups)
			system->Run();
		for (GenericSystem* func : GenericSystem::startups)
			if (func->callRelativity == After)
				func->fun();
	}

	void Update()
	{
		for (GenericSystem* func : GenericSystem::updates)
			if (func->callRelativity == Before)
				func->fun();
		for (System* system : System::updates)
			system->Run();
		for (GenericSystem* func : GenericSystem::updates)
			if (func->callRelativity == After)
				func->fun();
		// ADD PROPER DESTRUCTION OF ENTITIES HERE:
		//for (Entity& entity : entities)
		//	if (entity.)
	}

	void Close()
	{
		for (GenericSystem* func : GenericSystem::closes)
			if (func->callRelativity == Before)
				func->fun();
		for (System* system : System::closes)
			system->Run();
		for (GenericSystem* func : GenericSystem::closes)
			if (func->callRelativity == After)
				func->fun();
	}
};

void System::Run()
{
	if (!enabled) return;
	ForcedRun();
}

void System::ForcedRun()
{
	vector<vector<ProcEntity>> inputs = vector<vector<ProcEntity>>(requirements.size());
	for (int i = 0; i < requirements.size(); i++)
	{
		inputs[i] = vector<ProcEntity>(entities[i].size());

		for (int j = 0; j < entities[i].size(); j++)
		{
			Entity& entity = ECS::GetEntity(get<0>(entities[i][j]));
			inputs[i][j] = ProcEntity(requirements[i].requirements.size(), entity);
			for (int k = 0; k < requirements[i].requirements.size(); k++)
				inputs[i][j].components[k] = &entity.components[get<2>(entities[i][j])[k]];
		}
	}
	fun(inputs);
}