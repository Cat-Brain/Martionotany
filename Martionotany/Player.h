#pragma once
#include "Camera.h"
#include "Physics.h"

NewComponent(Player)
{
public:
	Position& camera;
	float accel, speed;

	Player(Position& camera, float accel, float speed) :
		camera(camera), accel(accel), speed(speed) { SET_HASH; }
};

NewComponent(PlayerJump)
{
public:
	float jumpForce;

	PlayerJump(float jumpForce) :
		jumpForce(jumpForce) { SET_HASH; }
};