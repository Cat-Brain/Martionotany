#pragma once
#include "ECSManager.h"

class Monitor
{
public:
	GLFWmonitor* monitor;
	ivec2 workPos, workScale;

	Monitor() :
		monitor(nullptr), workPos(0), workScale(1) { }

	void FindWorkArea()
	{
		glfwGetMonitorWorkarea(monitor, &workPos.x, &workPos.y, &workScale.x, &workScale.y);
	}
};

Monitor mainMonitor;