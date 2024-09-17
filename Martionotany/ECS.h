#pragma once
#include "Math.h"

#define HASH(x) (type_index(typeid(x)))
#define SET_HASH (hash_code = HASH(*this))
class BaseComponent
{
public:
	type_index hash_code = HASH(0);

	bool Cmp(const BaseComponent& other) const
	{
		return hash_code == other.hash_code;
	}
};

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

typedef vector<type_index> CompList;
typedef vector<CompList> SysReq;

class ECS;
class System
{
public:
	static vector<System*> startups, updates, closes, systems;
	static vector<pair<function<void()>, CallRelativity>> startupGenerics, updateGenerics, closeGenerics;

	SysReq requirements; // <- Values are the hash_codes of components
	function<void(vector<vector<vector<Component*>>>)> fun;
	vector<vector<std::pair<uint, vector<ushort>>>> entities;
	bool enabled;

	System(SysReq requirements, function<void(vector<vector<vector<Component*>>>)> fun, SystemCall call, bool enabled = true) :
		requirements(requirements), fun(fun), enabled(enabled), entities(requirements.size())
	{
		systems.push_back(this);
		switch (call)
		{
		case Start:
			System::startups.push_back(this);
			break;
		case Update:
			System::updates.push_back(this);
			break;
		case Close:
			System::closes.push_back(this);
			break;
		default:
			cout << "WARNING: System Call Invalid!!!\n";
			break;
		}
	}

	void Run(ECS* ecs);
};
vector<System*> System::startups{}, System::updates{}, System::closes{}, System::systems{};
vector<pair<function<void()>, CallRelativity>> System::startupGenerics{}, System::updateGenerics{}, System::closeGenerics{};

#define SYSTEM(name1, name2, requirements, call) void name1(vector<vector<vector<Component*>>> components); \
System name2(requirements, name1, call); \
void name1(vector<vector<vector<Component*>>> components)