#pragma once

#include <glad\glad.h>

#include <iostream>

class FrameBuffer
{
public:
	GLuint id;
	GLuint texColorBuffer;
	GLuint renderBuffer;

	GLfloat width, height;
	GLenum target;

	FrameBuffer(GLenum target, GLfloat w, GLfloat h);
	~FrameBuffer();
	void attachTexture(GLenum t);
	void attachRenderBuffer(GLenum t);
	void useFrameBuffer();
};

void useDefaultFrameBuffer(GLenum target);

FrameBuffer::FrameBuffer(GLenum t, GLfloat w, GLfloat h) :
width(w), height(h), target(t)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(t, id);

	attachTexture(t);
	attachRenderBuffer(t);

	if (glCheckFramebufferStatus(t) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
		std::abort();
	}

	glBindFramebuffer(t, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &id);
}

void FrameBuffer::attachTexture(GLenum target)
{
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
}

void FrameBuffer::attachRenderBuffer(GLenum target)
{
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(target, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
}

void FrameBuffer::useFrameBuffer()
{
	glBindFramebuffer(target, id);
}
void useDefaultFrameBuffer(GLenum target)
{
	glBindFramebuffer(target, 0);
}