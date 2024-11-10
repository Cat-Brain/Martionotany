#pragma once
#include "Math.h"

#define HASH(x) (CHash(typeid(x)))
#define SET_HASH (hash_code = HASH(*this))
class BaseComponent
{
public:
	CHash hash_code = HASH(0);
	bool toBeDestroy = false;

	bool Cmp(const BaseComponent& other) const
	{
		return hash_code == other.hash_code;
	}
};

#define NewComponent(Name) class Name : public BaseComponent

union Component;
class Entity;

enum SystemCall
{
	None, Start, Update, Close
};

enum CallRelativity
{
	Before, After
};

struct CompReq
{
	vector<CHash> requirements, tags, antiTags;

	CompReq(vector<CHash> requirements, vector<CHash> tags = {}, vector<CHash> = {}) :
		requirements(requirements), tags(tags), antiTags(antiTags) { }
};

struct CompWrapper
{
	Component& component;
	
	operator Component&()
	{
		return component;
	}
};

struct ProcEntity
{
	vector<Component*> components;
	Entity* entity;

	ProcEntity() :
		components(0), entity(nullptr) { }

	ProcEntity(vector<Component*> components, Entity* entity) :
		components(components), entity(entity) { }

	Component& operator[](int index)
	{
		return *components[index];
	}
};

typedef vector<CompReq> SysReq;

class GenericSystem
{
public:
	function<void()> fun;
	CallRelativity callRelativity;
	static vector<GenericSystem*> startups, updates, closes, systems;

	GenericSystem(function<void()> fun, CallRelativity callRelativity, SystemCall systemCall) :
		fun(fun), callRelativity(callRelativity)
	{
		systems.push_back(this);
		switch (systemCall)
		{
		case None:
			break;
		case Start:
			startups.push_back(this);
			break;
		case Update:
			updates.push_back(this);
			break;
		case Close:
			closes.push_back(this);
			break;
		default:
			cout << "WARNING: Generic System Call Invalid!!!\n";
			assert(false);
			break;
		}
	}
};
vector<GenericSystem*> GenericSystem::startups{}, GenericSystem::updates{}, GenericSystem::closes{}, GenericSystem::systems{};

#define GENERIC_SYSTEM(name1, name2, callRelativity, systemCall) void name1(); \
GenericSystem name2(name1, callRelativity, systemCall); \
void name1()


class System
{
public:
	static vector<System*> startups, updates, closes, systems;

	SysReq requirements; // <- Values are the hash_codes of components
	function<void(vector<vector<ProcEntity>>)> fun;
	vector<vector<tuple<uint, uint, vector<ushort>>>> entities; // [Entity index, Entity.systems index for this system, relevant component indices]
	bool enabled;

	System(SysReq requirements, function<void(vector<vector<ProcEntity>>)> fun, SystemCall call,
		bool enabled = true) :
		requirements(requirements), fun(fun), enabled(enabled), entities(requirements.size())
	{
		systems.push_back(this);
		switch (call)
		{
		case None:
			break;
		case Start:
			startups.push_back(this);
			break;
		case Update:
			updates.push_back(this);
			break;
		case Close:
			closes.push_back(this);
			break;
		default:
			cout << "WARNING: System Call Invalid!!!\n";
			assert(false);
			break;
		}
	}

	void Run();
	void ForcedRun();
};
vector<System*> System::startups{}, System::updates{}, System::closes{}, System::systems{};

#define SYSTEM_1(name1, name2, requirements, call)			System name2(requirements, name1, call);
#define SYSTEM_2(name1, name2, requirements, call, enabled)	System name2(requirements, name1, call, enabled);

#define SYSTEM_X(ignored, _1, fun, ...) fun

#define SYSTEM(name1, name2, requirements, call, ...) void name1(vector<vector<ProcEntity>> components); \
SYSTEM_X(, ##__VA_ARGS__, SYSTEM_2, SYSTEM_1)(name1, name2, requirements, call, ##__VA_ARGS__) \
void name1(vector<vector<ProcEntity>> components)