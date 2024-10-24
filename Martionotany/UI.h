#pragma once
#include "Framebuffer.h"

NewComponent(FollowCursor)
{
public:
	CameraMouse& mouse;

	FollowCursor(CameraMouse& mouse) :
		mouse(mouse) { SET_HASH; }
};