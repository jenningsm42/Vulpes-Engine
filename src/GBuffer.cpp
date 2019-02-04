#define VULPESENGINE_EXPORT

#include <GL/glew.h>

#include <vulpes/GBuffer.hpp>

#include "Logger.h"

namespace vul
{
	GBuffer::GBuffer(uint32_t textureCount) : m_textureCount(textureCount), m_loaded(false)
	{
	}

	GBuffer::~GBuffer()
	{
	}

	bool GBuffer::initialize(uint32_t windowWidth, uint32_t windowHeight, uint32_t scalingFactor)
	{
		if(m_loaded) release();

		m_windowWidth = windowWidth;
		m_windowHeight = windowHeight;
		m_scalingFactor = scalingFactor;

		// Create FBO
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		// Create G-Buffer textures
		glGenTextures(m_textureCount, m_textures);
		glGenTextures(1, &m_depthTexture);

		for(uint32_t i = 0; i < m_textureCount; i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, (i == 1)? GL_RG16F : GL_RGBA8, windowWidth * scalingFactor, windowHeight * scalingFactor, 0, (i == 1)? GL_RG : GL_RGBA, (i == 1)? GL_HALF_FLOAT : GL_BYTE, nullptr);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
		}

		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowWidth * scalingFactor, windowHeight * scalingFactor, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

		GLenum* drawBuffers = new GLenum[m_textureCount];
		for(uint32_t i = 0; i < m_textureCount; i++)
			drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

		glDrawBuffers(m_textureCount, drawBuffers);
		delete[] drawBuffers;

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			Logger::log("vul::GBuffer::initialize: Framebuffer error (0x%x)", status);
			return false;
		}

		// Restore default FBO
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		m_loaded = true;
		return true;
	}

	void GBuffer::release()
	{
		glDeleteTextures(1, &m_depthTexture);
		glDeleteTextures(m_textureCount, m_textures);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_fbo);
		m_loaded = false;
	}

	void GBuffer::bindWrite()
	{
		glViewport(0, 0, m_windowWidth * m_scalingFactor, m_windowHeight * m_scalingFactor);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	}

	void GBuffer::endWrite()
	{
		glViewport(0, 0, m_windowWidth, m_windowHeight);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Restore default framebuffer (screen/window)
	}

	void GBuffer::bindRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	}

	void GBuffer::setReadBuffer(uint32_t index)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
	}

	void GBuffer::bindTexture(uint32_t index)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[index]);
	}
}
