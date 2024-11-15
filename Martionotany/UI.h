#pragma once
#include "Framebuffer.h"
#include "Input.h"

NewComponent(FollowCursor)
{
public:
	CameraMouse& mouse;

	FollowCursor(CameraMouse& mouse) :
		mouse(mouse) { SET_HASH; }
};

// Requires some collider to function properly
NewComponent(MouseInteractable)
{
public:
	bool hovered, pressed, held, released;

	MouseInteractable() :
		hovered(false), pressed(false), held(false), released(false) { SET_HASH; }

	void Update(bool hovered)
	{
		this->hovered = hovered;
		released = Input::click1.released && hovered && held;
		pressed = Input::click1.pressed && hovered;
		held = pressed || (held && Input::click1.held && hovered);
	}
};

NewComponent(InteractableColor)
{
public:
	vec4 normal, hovered, held;

	InteractableColor(vec4 normal, vec4 hovered, vec4 held):
		normal(normal), hovered(hovered), held(held) { SET_HASH; }
};

NewTag(DestroyOnInteract);

NewComponent(TestPrintOnInteract)
{
public:
	char text;

	TestPrintOnInteract(char text) :
		text(text) { SET_HASH; }
};