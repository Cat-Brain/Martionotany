#pragma once
#include "ECS.h"

namespace Input
{
	struct Key
	{
	public:
		int keycode;
		bool pressed = false, held = false, released = false;

		Key(int keycode);

		void Update()
		{
			bool nowHeld = glfwGetKey(window, keycode) == GLFW_PRESS;
			pressed = nowHeld && !held;
			released = !nowHeld && held;
			held = nowHeld;
		}
	};

	vector<Key*> keys = vector<Key*>(0);
	Key w(GLFW_KEY_W), s(GLFW_KEY_S), d(GLFW_KEY_D), a(GLFW_KEY_A),
		jump(GLFW_KEY_SPACE), escape(GLFW_KEY_ESCAPE);


	Key::Key(int keycode) :
		keycode(keycode)
	{
		keys.push_back(this);
	}

	void UpdateKeys()
	{
		for (Key* key : keys)
			key->Update();

		if (escape.held)
			glfwSetWindowShouldClose(window, true);
	}
}