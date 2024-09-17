#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Player.h"

#define TYPE(type, name) type name; \
Component(type&& name) : \
	name(name) { }

union Component
{
	TYPE(BaseComponent, base);
	TYPE(MeshRenderer, meshRenderer);
	TYPE(MeshScale, meshScale);
	TYPE(Camera, camera);
	TYPE(Position, position);
	TYPE(PhysicsBody, physicsBody);
	TYPE(PhysicsCircle, physicsCircle);
	TYPE(InfinitePhysicsWall, infinitePhysicsWall);
	TYPE(Gravity, gravity);
	TYPE(Player, player);
};

class Entity
{
public:
	bool firstFrame;
	vector<Component> components;
	vector<int> systems;

	Entity(vector<Component> components, bool firstFrame = true) :
		firstFrame(firstFrame), components(components), systems{} { }

	bool HasComponents(vector<type_index>& requirements)
	{
		for (type_index requirement : requirements)
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
		return true;
	}
};

class ECS
{
	friend class System;
protected:
	vector<Entity> entities;

public:
	void AddEntity(Entity&& entity)
	{
		uint entityId = static_cast<uint>(entities.size());
		entities.push_back(entity);
		for (System* system : System::systems)
			for (int i = 0; i < system->requirements.size(); i++)
				if (entity.HasComponents(system->requirements[i]))
				{
					uint index = static_cast<uint>(system->entities[i].size());
					system->entities[i].push_back({ entityId, vector<ushort>(system->requirements[i].size())});
					for (int j = 0; j < system->requirements[i].size(); j++)
						for (uint k = 0; k < entity.components.size(); k++)
							if (entity.components[k].base.hash_code == system->requirements[i][j])
							{
								system->entities[i][index].second[j] = k;
								break;
							}

					// Add stuff to add systems onto entities instead of just entities onto systems
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
		for (pair<function<void()>, CallRelativity>& func : System::startupGenerics)
			if (func.second == Before)
				func.first();
		for (System* system : System::startups)
			system->Run(this);
		for (pair<function<void()>, CallRelativity>& func : System::startupGenerics)
			if (func.second == After)
				func.first();
	}

	void Update()
	{
		for (pair<function<void()>, CallRelativity>& func : System::updateGenerics)
			if (func.second == Before)
				func.first();
		for (System* system : System::updates)
			system->Run(this);
		for (pair<function<void()>, CallRelativity>& func : System::updateGenerics)
			if (func.second == After)
				func.first();
	}

	void Close()
	{
		for (pair<function<void()>, CallRelativity>& func : System::closeGenerics)
			if (func.second == Before)
				func.first();
		for (System* system : System::closes)
			system->Run(this);
		for (pair<function<void()>, CallRelativity>& func : System::closeGenerics)
			if (func.second == After)
				func.first();
	}
};

void System::Run(ECS* ecs)
{
	vector<vector<vector<Component*>>> inputs = vector<vector<vector<Component*>>>(requirements.size());
	for (int i = 0; i < requirements.size(); i++)
	{
		inputs[i] = vector<vector<Component*>>(entities[i].size());

		for (int j = 0; j < entities[i].size(); j++)
		{
			Entity& entity = ecs->GetEntity(entities[i][j].first);
			inputs[i][j] = vector<Component*>(requirements[i].size());
			for (int k = 0; k < requirements[i].size(); k++)
				inputs[i][j][k] = &entity.components[entities[i][j].second[k]];
		}
	}
	fun(inputs);
}