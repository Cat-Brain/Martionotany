#pragma once
#include "ECS.h"

class Position : public BaseComponent
{
public:
	vec2 pos;

	Position(vec2 pos = vec2(0)) :
		pos(pos) { SET_HASH; }
};

// Requires Position
class PhysicsBody : public BaseComponent
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
};

class InfinitePhysicsWall : public BaseComponent
{
public:
	vec2 normal;
	float height; // In direction of normal from origin

	InfinitePhysicsWall(vec2 normal, float height) :
		normal(normal), height(height) { SET_HASH; }
};

constexpr float defaultGravity = 10;
class Gravity : public BaseComponent
{
public:
	float multiplier;
	vec2 direction;

	Gravity(float multiplier = 1, vec2 direction = Vec::down):
		multiplier(multiplier), direction(direction) { SET_HASH; }
};