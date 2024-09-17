#pragma once
#include "ECS.h"

namespace RenderLayer
{
	enum
	{
		DEFAULT = 1, FRAMEBUFFER = 2
	};
}

class Camera : public BaseComponent
{
public:
	float zoom;
	mat4 matrix;
	Shader& shader;
	byte renderMask;

	Camera(float zoom, byte renderMask, Shader& shader = defaultShader) :
		zoom(zoom), shader(shader), matrix(glm::identity<mat4>()), renderMask(renderMask) { SET_HASH; }
};

class Framebuffer : public BaseComponent
{
public:
	uint fbo;

	Framebuffer()// :

	{
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);


		SET_HASH;
	}
};