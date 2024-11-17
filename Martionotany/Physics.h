#pragma once
#include "ECS.h"

NewComponent(Position)
{
public:
	vec2 pos;

	Position(vec2 pos = vec2(0)) :
		pos(pos) { SET_HASH; }

	float Distance(Position position)
	{
		return glm::distance(pos, position.pos);
	}

	float Distance2(Position position)
	{
		return glm::distance2(pos, position.pos);
	}
};

// Requires Position
NewComponent(PhysicsBody)
{
public:
	float mass;
	vec2 vel;

	PhysicsBody(float mass, vec2 vel = vec2(0)) :
		mass(mass), vel(vel) { SET_HASH; assert(mass > 0); }
};

class PhysicsCircle : public BaseComponent
{
public:
	float radius;

	PhysicsCircle(float radius = 0.5f) :
		radius(radius) { SET_HASH; }

	bool Overlaps(vec2 relPos) const
	{
		return glm::length2(relPos) <= radius * radius;
	}
};

NewComponent(PhysicsBox)
{
public:
	vec2 dimensions;

	PhysicsBox(vec2 dimensions = vec2(1)) :
		dimensions(dimensions) { SET_HASH; }

	bool Overlaps(vec2 relPos) const
	{
		relPos += dimensions * 0.5f;
		return relPos.x >= 0 && relPos.x <= dimensions.x && relPos.y >= 0 && relPos.y <= dimensions.y;
	}
};

NewComponent(InfinitePhysicsWall)
{
public:
	vec2 normal;
	float height; // In direction of normal from origin

	InfinitePhysicsWall(vec2 normal, float height) :
		normal(normal), height(height) { SET_HASH; }
};

constexpr float defaultGravity = 10;
NewComponent(Gravity)
{
public:
	float multiplier;
	vec2 direction;

	Gravity(float multiplier = 1, vec2 direction = Vec::down):
		multiplier(multiplier), direction(direction) { SET_HASH; }
};