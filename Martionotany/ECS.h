#pragma once
#include "Math.h"

#define HASH(x) (CHash(typeid(x)))
#define SET_HASH (hash_code = HASH(*this))
#define HASH_ALL_BODY(n, d, x) HASH(x), 
#define HASH_ALL(seq) { FOR_EACH(HASH_ALL_BODY, seq) }

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

#define NewComponent(Name) \
class Name : public BaseComponent

#define NewTag(Name) NewComponent(Name) \
{ \
public: \
	Name() \
	{ \
		SET_HASH; \
	} \
}

NewTag(NotEnabledOnAwaken);

union Component;
class Entity;


template <typename T>
class Storer
{
public:
	vector<T> data;
	vector<int*> locations;

	void NewStorageCell(int* location, const T toStore = {})
	{
		assert(location != nullptr);
		*location = data.size();
		locations.push_back(location);
		data.push_back(toStore);
	}

	T& Get(int storageIndex)
	{
		return data[storageIndex];
	}
};

Storer<vector<Entity*>> entityStorage;
Storer<string> stringStorage;


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


typedef function<bool(Entity&)> EntityEval;
bool DefaultEntityEval(Entity& entity);

struct CompReq
{
	vector<CHash> requirements, tags, antiTags, optionals;
	EntityEval eval;

	CompReq(vector<CHash> requirements = {}, vector<CHash> tags = {}, vector<CHash> antiTags = {},
		vector<CHash> optionals = {}, EntityEval eval = DefaultEntityEval) :
		requirements(requirements), tags(tags), antiTags(antiTags), optionals(optionals), eval(eval) { }

	size_t ReqCount() const
	{
		return requirements.size() + optionals.size();
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

	bool ComponentFound(int index)
	{
		return components[index] != nullptr;
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


class System
{
public:
	static vector<System*> systems;
	static vector<vector<System*>> sortedSystems;

	SysReq requirements; // Values are the hash_codes of components
	function<void(vector<vector<ProcEntity>>)> fun;
	// [Entity index, Entity.systems index for this system, relevant component indices]
	vector<vector<tuple<uint, uint, vector<short>>>> entities;
	bool enabled;
	SystemCall call;

		System(SysReq requirements, function<void(vector<vector<ProcEntity>>)> fun, SystemCall call,
		bool enabled = true) :
		requirements(requirements), fun(fun), call(call), enabled(enabled),
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
#define SYSTEM_2(name1, name2, requirements, call, enabled)	System name2(requirements, name1, call, enabled);

#define SYSTEM_X(ignored, _1, fun, ...) fun

#define SYSTEM(name1, name2, requirements, call, ...) void name1(vector<vector<ProcEntity>> components); \
SYSTEM_X(, ##__VA_ARGS__, SYSTEM_2, SYSTEM_1)(name1, name2, requirements, call, ##__VA_ARGS__) \
void name1(vector<vector<ProcEntity>> components)

#define COMP_REQ_1(requirements)							(CompReq(HASH_ALL(requirements)))
#define COMP_REQ_2(requirements, tags)						(CompReq(HASH_ALL(requirements), HASH_ALL(tags)))
#define COMP_REQ_3(requirements, tags, antiTags)			(CompReq(HASH_ALL(requirements), HASH_ALL(tags), HASH_ALL(antiTags)))
#define COMP_REQ_4(requirements, tags, antiTags, optionals)	(CompReq(HASH_ALL(requirements), HASH_ALL(tags), HASH_ALL(antiTags),\
	HASH_ALL(optionals)))
#define COMP_REQ_5(requirements, tags, antiTags, optionals, eval) (CompReq(HASH_ALL(requirements), HASH_ALL(tags), \
	HASH_ALL(antiTags), HASH_ALL(optionals), eval))

#define COMP_REQ_X(ignored, _1, _2, _3, _4, fun, ...) fun

// requirements, tags, antiTags, optionals, eval
#define COMP_REQ(...) COMP_REQ_X(__VA_ARGS__, COMP_REQ_5, COMP_REQ_4, COMP_REQ_3, COMP_REQ_2, COMP_REQ_1)(__VA_ARGS__)


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