#pragma once
#include "Camera.h"
#include "Physics.h"

class Player : public BaseComponent
{
public:
	Position& camera;
	float accel, speed;
	float jumpForce;

	Player(Position& camera, float accel, float speed, float jumpForce) :
		camera(camera), accel(accel), speed(speed), jumpForce(jumpForce) { SET_HASH; }
};