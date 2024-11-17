#pragma once
#include "ECS.h"

NewComponent(MoveToPlayer)
{
public:
	float speed;

	MoveToPlayer(float speed) :
		speed(speed) {
		SET_HASH;
	}
};