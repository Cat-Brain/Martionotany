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
	Start, Update, Close
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
			break;
		}
	}
};
vector<GenericSystem*> GenericSystem::startups{}, GenericSystem::updates{}, GenericSystem::closes{}, GenericSystem::systems{};

#define GENERIC_SYSTEM(name1, name2, callRelativity, systemCall) void name1(); \
GenericSystem name2(name1, callRelativity, systemCall); \
void name1()


class ECS;
class System
{
public:
	static vector<System*> startups, updates, closes, systems;

	SysReq requirements; // <- Values are the hash_codes of components
	function<void(vector<vector<vector<Component*>>>)> fun;
	vector<vector<tuple<uint, uint, vector<ushort>>>> entities; // [Entity index, Entity.systems index for this system, relevant component indices]
	bool enabled;

	System(SysReq requirements, function<void(vector<vector<vector<Component*>>>)> fun, SystemCall call, bool enabled = true) :
		requirements(requirements), fun(fun), enabled(enabled), entities(requirements.size())
	{
		systems.push_back(this);
		switch (call)
		{
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
			break;
		}
	}

	void Run(ECS* ecs);
};
vector<System*> System::startups{}, System::updates{}, System::closes{}, System::systems{};

#define SYSTEM(name1, name2, requirements, call) void name1(vector<vector<vector<Component*>>> components); \
System name2(requirements, name1, call); \
void name1(vector<vector<vector<Component*>>> components)