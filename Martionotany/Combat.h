#pragma once
#include "ECS.h"

NewComponent(Health)
{
public:
	int health;

	Health(int health) :
		health(health) { SET_HASH; }
};

NewComponent(CustomDeath)
{
public:
	bool dying;

	CustomDeath() :
		dying(false) { SET_HASH; }
};

NewComponent(Direction)
{
public:
	vec2 dir;

	Direction(vec2 dir = vec2(0)) :
		dir(dir) { SET_HASH; }
};

NewTag(SetVelToDir);

// Requires PhysicsBody and PhysicsTrigger to function normally
NewComponent(Projectile)
{
public:
	float speed, range, duration;
	int damage, pierce;

	Projectile(float speed, float range, int damage, int pierce = 1) :
		speed(speed), range(range), duration(range / speed), damage(damage), pierce(pierce) { SET_HASH; }
};

NewComponent(PointAward)
{
public:
	int points;

	PointAward(int points) :
		points(points) { SET_HASH; }
};

NewComponent(PlayerPoints)
{
public:
	int points;

	PlayerPoints(int points = 0) :
		points(points) { SET_HASH; }
};