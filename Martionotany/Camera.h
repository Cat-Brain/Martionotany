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

NewComponent(Camera)
{
public:
	mat4 matrix, debugMatrix;
	Framebuffer* framebuffer;
	Shader& shader;
	byte renderMask;

	Camera(Framebuffer* framebuffer, byte renderMask, Shader& shader = defaultShader) :
		framebuffer(framebuffer), shader(shader),
		matrix(glm::identity<mat4>()), debugMatrix(glm::identity<mat4>()),
		renderMask(renderMask) { SET_HASH; }

	vec2 CamDim() const; // Defined after Framebuffer
};

NewComponent(CameraMouse)
{
public:
	vec2 camMousePos, worldMousePos, gridMousePos;

	CameraMouse() :
		camMousePos(0), worldMousePos(0), gridMousePos(0) { SET_HASH; }
};