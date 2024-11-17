#pragma once
#include "ECS.h"

namespace RenderLayer
{
	enum
	{
		DEFAULT = 1, FRAMEBUFFER = 2
	};
}

class Framebuffer;

// Requires CameraMatrix
NewComponent(Camera)
{
public:
	Framebuffer* framebuffer;
	Shader& shader;
	byte renderMask;

	Camera(Framebuffer* framebuffer, byte renderMask, Shader& shader = defaultShader) :
		framebuffer(framebuffer), shader(shader),
		renderMask(renderMask) { SET_HASH; }

	vec2 CamDim() const; // Defined after Framebuffer

	vec2 ScreenToCameraSpace(vec2 original);
};

// Requires Camera
NewComponent(CameraMatrix)
{
public:
	mat4 matrix, debugMatrix;

	CameraMatrix() :
		matrix(identity<mat4>()), debugMatrix(identity<mat4>()) { SET_HASH; }
};

NewComponent(CameraMouse)
{
public:
	vec2 camMousePos, worldMousePos, gridMousePos;

	CameraMouse() :
		camMousePos(0), worldMousePos(0), gridMousePos(0) { SET_HASH; }
};