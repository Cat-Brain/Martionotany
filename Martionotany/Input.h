#pragma once
#include "ECS.h"

namespace Input
{
	enum KeyType
	{
		Keyboard, Mouse
	};

	struct Key
	{
	public:
		int keycode;
		KeyType keyType;
		bool pressed = false, held = false, released = false;

		Key(int keycode, KeyType keyType = Keyboard);

		void Update()
		{
			bool nowHeld;
			// Maybe should be made to a start switch instead of a update switch
			switch (keyType)
			{
			case Keyboard:
				nowHeld = glfwGetKey(window, keycode) == GLFW_PRESS;
				break;
			case Mouse:
				nowHeld = glfwGetMouseButton(window, keycode) == GLFW_PRESS;
				break;
			default:
				return;
			};
			pressed = nowHeld && !held;
			released = !nowHeld && held;
			held = nowHeld;
		}
	};

	vector<Key*> keys = vector<Key*>(0);
	Key w(GLFW_KEY_W), s(GLFW_KEY_S), d(GLFW_KEY_D), a(GLFW_KEY_A),
		jump(GLFW_KEY_SPACE), sprint(GLFW_KEY_LEFT_SHIFT), walk(GLFW_KEY_LEFT_CONTROL),
		escape(GLFW_KEY_ESCAPE),
		enterDebug(GLFW_KEY_ENTER),
		click1(GLFW_MOUSE_BUTTON_1, Mouse), click2(GLFW_MOUSE_BUTTON_2, Mouse);

	vec2 pixMousePos, screenMousePos;


	Key::Key(int keycode, KeyType keyType) :
		keycode(keycode), keyType(keyType)
	{
		keys.push_back(this);
	}

	GENERIC_SYSTEM(UpdateKeys, updateKeys, Before, Update)
	{
		for (Key* key : keys)
			key->Update();

		if (escape.held)
			glfwSetWindowShouldClose(window, true);
	}

	GENERIC_SYSTEM(UpdateInDebug, updateInDebug, Before, Update)
	{
		inDebug ^= enterDebug.pressed;
	}

	SYSTEM(UpdateMouse, updateMouse, { {} }, Update)
	{
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		pixMousePos = vec2(xPos, yPos);
		screenMousePos = vec2(pixMousePos.x / (screenDim.x - 1), (screenDim.y - 1 - pixMousePos.y) / (screenDim.y - 1));
	}
}