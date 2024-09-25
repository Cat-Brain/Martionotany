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
	mat4 matrix;
	Framebuffer* framebuffer;
	Shader& shader;
	byte renderMask;

	Camera(Framebuffer* framebuffer, byte renderMask, Shader& shader = defaultShader) :
		framebuffer(framebuffer), shader(shader), matrix(glm::identity<mat4>()), renderMask(renderMask) { SET_HASH; }
};