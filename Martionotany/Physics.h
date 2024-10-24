#pragma once
#include "ECS.h"

NewComponent(Position)
{
public:
	vec2 pos;

	Position(vec2 pos = vec2(0)) :
		pos(pos) { SET_HASH; }
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

	PhysicsCircle(float radius) :
		radius(radius) { SET_HASH; }

	bool Overlaps(vec2 relPos)
	{
		return glm::length2(relPos) <= radius * radius;
	}
};

NewComponent(PhysicsBox)
{
public:
	vec2 dimensions;
	float rotation;

	PhysicsBox(vec2 dimensions = vec2(1), float rotation = 0) :
		dimensions(dimensions), rotation(rotation) { SET_HASH; }
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

// Requires some collider to function properly
NewComponent(MouseInteractable)
{
public:
	bool pressed, held, released;

	MouseInteractable():
		pressed(false), held(false), released(false) { SET_HASH; }

	void Update(bool hovered)
	{
		released = Input::click1.released && hovered && held;
		pressed = Input::click1.pressed && hovered;
		held = pressed || (held && Input::click1.held && hovered);
	}
};