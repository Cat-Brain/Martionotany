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
	Start, // First frame window is open
	Update, // Every frame
	Close, // Last frame window is open
	SystemCallCount, // How many system call types there is. An invalid system call itself
	None // Never called by generic system calling methods, only by specifics
};

enum CallRelativity
{
	Before, After
};

struct CompReq
{
	vector<CHash> requirements, tags, antiTags;

	CompReq(vector<CHash> requirements = {}, vector<CHash> tags = {}, vector<CHash> = {}) :
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
	SystemCall call;

	static vector<GenericSystem*> systems;
	static vector<vector<GenericSystem*>> sortedSystems;

	GenericSystem(function<void()> fun, CallRelativity callRelativity, SystemCall call) :
		fun(fun), callRelativity(callRelativity), call(call)
	{
		systems.push_back(this);
		if (call == None)
			return;
		if (call < 0 || call == SystemCallCount || call > None)
		{
			cout << "WARNING: Generic System Call Invalid!!!\n";
			assert(false);
			return;
		}
		sortedSystems[call].push_back(this);
	}
};
vector<GenericSystem*> GenericSystem::systems{};
vector<vector<GenericSystem*>> GenericSystem::sortedSystems(SystemCallCount);

#define GENERIC_SYSTEM(name1, name2, callRelativity, systemCall) void name1(); \
GenericSystem name2(name1, callRelativity, systemCall); \
void name1()


typedef function<bool(Entity&)> EntityEval;
bool DefaultEntityEval(Entity& entity);

class System
{
public:
	static vector<System*> systems;
	static vector<vector<System*>> sortedSystems;

	SysReq requirements; // <- Values are the hash_codes of components
	function<void(vector<vector<ProcEntity>>)> fun;
	vector<vector<tuple<uint, uint, vector<ushort>>>> entities; // [Entity index, Entity.systems index for this system, relevant component indices]
	bool enabled;
	SystemCall call;
	EntityEval eval;

		System(SysReq requirements, function<void(vector<vector<ProcEntity>>)> fun, SystemCall call,
		EntityEval eval = DefaultEntityEval, bool enabled = true) :
		requirements(requirements), fun(fun), call(call), enabled(enabled), eval(eval),
			entities(requirements.size())
	{
		systems.push_back(this);
		if (call == None)
			return;
		if (call < 0 || call == SystemCallCount || call > None)
		{
			cout << "WARNING: System Call Invalid!!!\n";
			assert(false);
			return;
		}
		sortedSystems[call].push_back(this);
	}

	void Run();
	void ForcedRun();
};
vector<System*> System::systems{};
vector<vector<System*>> System::sortedSystems(SystemCallCount);

#define SYSTEM_1(name1, name2, requirements, call)					System name2(requirements, name1, call);
#define SYSTEM_2(name1, name2, requirements, call, eval)			System name2(requirements, name1, call, eval);
#define SYSTEM_3(name1, name2, requirements, call, eval, enabled)	System name2(requirements, name1, call, eval, enabled);

#define SYSTEM_X(ignored, _1, _2, fun, ...) fun

#define SYSTEM(name1, name2, requirements, call, ...) void name1(vector<vector<ProcEntity>> components); \
SYSTEM_X(, ##__VA_ARGS__, SYSTEM_3, SYSTEM_2, SYSTEM_1)(name1, name2, requirements, call, ##__VA_ARGS__) \
void name1(vector<vector<ProcEntity>> components)


void CallSystems(SystemCall call)
{
	if (call == None)
		return;
	if (call < 0 || call == SystemCallCount || call > None)
	{
		cout << "WARNING: System Call Invalid!!!\n";
		assert(false);
		return;
	}


	for (GenericSystem* func : GenericSystem::sortedSystems[call])
		if (func->callRelativity == Before)
			func->fun();
	for (System* system : System::sortedSystems[call])
		system->Run();
	for (GenericSystem* func : GenericSystem::sortedSystems[call])
		if (func->callRelativity == After)
			func->fun();
}