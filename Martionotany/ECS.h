#pragma once
#include "Math.h"

#define HASH(x) (CHash(typeid(x)))
#define SET_HASH (hash_code = HASH(*this))
class BaseComponent
{
public:
	CHash hash_code = HASH(0);

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

typedef vector<CHash> CompList;
typedef vector<CompList> SysReq;

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
	SysReq tags, antiTags; // Tags are requirements that don't need to be passed, antitags are requirements of something that mustn't be there
	function<void(vector<vector<vector<Component*>>>)> fun;
	vector<vector<tuple<uint, uint, vector<ushort>>>> entities; // [Entity index, Entity.systems index for this system, relevant component indices]
	bool enabled;

	System(SysReq requirements, function<void(vector<vector<vector<Component*>>>)> fun, SystemCall call,
		SysReq tags = { {} }, SysReq antiTags = { {} }, bool enabled = true) :
		requirements(requirements), fun(fun),
		tags(tags), antiTags(antiTags), enabled(enabled), entities(requirements.size())
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
};
vector<System*> System::startups{}, System::updates{}, System::closes{}, System::systems{};

#define SYSTEM_1(name1, name2, requirements, call)								System name2(requirements, name1, call);
#define SYSTEM_2(name1, name2, requirements, call, tags)						System name2(requirements, name1, call, tags);
#define SYSTEM_3(name1, name2, requirements, call, tags, antiTags)				System name2(requirements, name1, call, tags, antiTags);
#define SYSTEM_4(name1, name2, requirements, call, tags, antiTags, enabled)		System name2(requirements, name1, call, tags, antiTags, enabled);

#define GET_5TH_ARG(arg1, arg2, arg3, arg4, arg5, ...) arg5
#define SYSTEM_MACRO_CHOOSER(...) \
    GET_5TH_ARG(__VA_ARGS__, SYSTEM_4, SYSTEM_3, \
                SYSTEM_2, SYSTEM_1)

#define SYSTEM(name1, name2, requirements, call, ...) void name1(vector<vector<vector<Component*>>> components); \
SYSTEM_MACRO_CHOOSER(__VA_ARGS__)(name1, name2, requirements, call, __VA_ARGS__) \
void name1(vector<vector<vector<Component*>>> components)