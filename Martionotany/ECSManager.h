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
	TYPE(Player, player);

	constexpr Component& operator=(const Component& component)
	{
		return *this = component;
	}
};

class Entity
{
public:
	bool enabled, firstFrame, toDestroy;
	vector<Component> components;
	vector<tuple<System*, uint, uint>> systems; // [System, Outer Index (Which entity parameter this meets), Inner Index (Which entity is this)
	int index = 0;

	Entity(vector<Component> components, bool enabled = true, bool firstFrame = true, bool toDestroy = false) :
		components(components), enabled(enabled), firstFrame(firstFrame), toDestroy(toDestroy), systems{} { }

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

	// Bugs out whenever component not found and returns an error
	Component& GetComponent(CHash desired)
	{
		for (Component& component : components)
			if (component.base.hash_code == desired)
				return component;
		assert(false);
		return components[0];
	}

	// Return nullptr when not found
	Component* TryGetComponent(CHash desired)
	{
		for (Component& component : components)
			if (component.base.hash_code == desired)
				return &component;
		return nullptr;
	}
	// Return nullptr when not found
	template <class T>
	T* TryGetComponent()
	{
		// Maybe put the correct cast type here someday lol
		return (T*)TryGetComponent(HASH(T));
	}

	void SetIndex(int index)
	{
		this->index = index;
		for (auto& [system, outerIndex, innerIndex] : systems)
			get<0>(system->entities[outerIndex][innerIndex]) = index;
	}
};

namespace ECS
{
	vector<Entity> entities{};

	int AddEntity(Entity&& entity)
	{
		uint entityId = static_cast<uint>(entities.size());
		entities.push_back(entity);
		entities[entityId].index = entityId;

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
					entities[entityId].systems.push_back({system, i, index});
				}
		return static_cast<int>(entities.size() - 1);
	}

	void RemoveEntity(uint index)
	{
		Entity& entity = entities[index];

		for (auto& [system, outerIndex, innerIndex] : entity.systems)
		{
			system->entities[outerIndex].erase(system->entities[outerIndex].begin() + innerIndex);
			for (int i = innerIndex; i < system->entities[outerIndex].size(); i++)
			{
				get<2>(entities[get<0>(system->entities[outerIndex][i])].systems[get<1>(system->entities[outerIndex][i])]) = i;
				//get<0>(system->entities[outerIndex][i])--;
			}
		}

		entities.erase(entities.begin() + index);

		for (int i = index; i < entities.size(); i++)
			entities[i].SetIndex(i);
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
		CallSystems(SystemCall::Start);
	}

	void Update()
	{
		CallSystems(SystemCall::Update);
	}

	void Close()
	{
		CallSystems(SystemCall::Close);
	}
};


#pragma region System Call Evals
// System call evals can go here, they must be after entity:
bool DefaultEntityEval(Entity& entity)
{
	return entity.enabled;
}

bool AwakeEntityEval(Entity& entity)
{
	return entity.firstFrame;
}
bool DestroyEntityEval(Entity& entity)
{
	return entity.toDestroy;
}

bool OnClickEntityEval(Entity& entity)
{
	MouseInteractable* interactable = entity.TryGetComponent<MouseInteractable>();
	if (interactable == nullptr)
		return false;
	return interactable->pressed;
}
bool OnReleaseEntityEval(Entity& entity)
{
	MouseInteractable* interactable = entity.TryGetComponent<MouseInteractable>();
	if (interactable == nullptr)
		return false;
	return interactable->released;
}
#pragma endregion

void System::Run()
{
	if (!enabled) return;
	ForcedRun();
}

void System::ForcedRun()
{
	vector<vector<ProcEntity>> inputs = vector<vector<ProcEntity>>(requirements.size());
	for (int i = 0; i < requirements.size(); i++)
		for (int j = 0; j < entities[i].size(); j++)
		{
			// Gets reference to entity to avoid putting this horrible piece of code everywhere
			Entity& entity = ECS::GetEntity(get<0>(entities[i][j]));

			// Skip if entity does not meet eval requirements
			if (!eval(entity))
				continue;

			// Add new ProcEntity with for the entity and set up its components vector
			inputs[i].push_back(ProcEntity(vector<Component*>(requirements[i].requirements.size()), &entity));

			// Add all the components in
			for (int k = 0; k < requirements[i].requirements.size(); k++)
				inputs[i][inputs[i].size() - 1].components[k] = &entity.components[get<2>(entities[i][j])[k]];
		}
	fun(inputs);
}