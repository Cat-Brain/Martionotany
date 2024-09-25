#pragma once
#include "Camera.h"
#include "Mesh.h"

class Framebuffer
{
public:
	static vector<Framebuffer*> framebuffers;
	
	int ppu;
	float zoom, desiredHeight;
	ivec2 dim;
	bool fixedDimensions;
	uint fbo, texture, rbo;

	Framebuffer(int ppu, float zoom, bool fixedDimensions = false) :
		ppu(ppu), zoom(zoom), desiredHeight(0), dim(0), fixedDimensions(fixedDimensions),
		fbo(999), texture(999), rbo(999)
	{
		framebuffers.push_back(this);
	}

	void Init()
	{
		FindSetDesiredHeight();
		dim = FindDim();

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);


		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim.x, dim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dim.x, dim.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Resize()
	{
		FindSetDesiredHeight();
		ivec2 newDim = FindDim();
		if (dim == newDim) return;
		dim = newDim;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim.x, dim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glDeleteRenderbuffers(1, &rbo);
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dim.x, dim.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	void Terminate()
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &texture);
		glDeleteRenderbuffers(1, &rbo);
	}

	void FindSetDesiredHeight()
	{
		desiredHeight = ppu * zoom * 2;
	}

	const int ScaleFactor()
	{
		return static_cast<int>(ceilf(static_cast<float>(screenDim.y / desiredHeight)));
	}

	ivec2 FindDim()
	{
		int factor = ScaleFactor();
		return ivec2(ceil(static_cast<float>(screenDim.x / factor)), ceil(static_cast<float>(screenDim.y / factor)));
	}

	vec2 FindStretch()
	{
		int factor = ScaleFactor();
		return vec2(static_cast<float>(screenDim.x) / (factor * dim.x), static_cast<float>(screenDim.y) / (factor * dim.y));
	}

	const void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, dim.x, dim.y);
	}

	static void BindScreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, screenDim.x, screenDim.y);
	}
};

vector<Framebuffer*> Framebuffer::framebuffers = vector<Framebuffer*>();
Framebuffer mainFramebuffer(pixelsPerUnit, 5);

void FramebufferInit()
{
	for (Framebuffer* framebuffer : Framebuffer::framebuffers)
		framebuffer->Init();
}

void FramebufferResize()
{
	for (Framebuffer* framebuffer : Framebuffer::framebuffers)
		framebuffer->Resize();
}

void RenderToFramebuffer()
{
	mainFramebuffer.Bind();

	glClearColor(0.3f, 0.3f, 0.6f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderToScreen()
{
	Framebuffer::BindScreen();
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainFramebuffer.texture);

	glUseProgram(framebufferShader.program);
	vec2 stretch = mainFramebuffer.FindStretch();
	glUniform2f(glGetUniformLocation(framebufferShader.program, "stretch"),
		stretch.x, stretch.y);

	quadMesh.Draw();
}

void FramebufferTerminate()
{
	for (Framebuffer* framebuffer : Framebuffer::framebuffers)
		framebuffer->Terminate();
}