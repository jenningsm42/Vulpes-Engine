#define VULPESENGINE_EXPORT
#include "include/RenderTarget.h"
#include "include/ResourceCache.h"
#include "Logger.h"
#include <GL/glew.h>

namespace vul
{
	RenderTarget::RenderTarget(uint32_t width, uint32_t height, uint32_t numTargets)
		: m_width(width), m_height(height), m_numTargets(numTargets), m_loaded(false)
	{
		glGetIntegerv(GL_VIEWPORT, m_defaultViewport);
		m_loaded = initialize();
	}

	RenderTarget::~RenderTarget()
	{
		release();
	}

	void RenderTarget::beginWrite()
	{
		glViewport(0, 0, m_width, m_height);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	}

	void RenderTarget::endWrite()
	{
		glViewport(m_defaultViewport[0], m_defaultViewport[1], m_defaultViewport[2], m_defaultViewport[3]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	Handle<Texture> RenderTarget::getTexture(uint32_t index)
	{
		return m_textures[index];
	}

	bool RenderTarget::cacheTexture(const std::string& name, Handle<ResourceCache> rc, uint32_t index)
	{
		if(rc->hasResource(name)) return false;
		rc->addTexture(name, m_textures[index]);
		m_texturesCached[index] = true;
		return true;
	}

	bool RenderTarget::addTarget(Handle<Texture>& texture, uint32_t target, uint32_t level)
	{
		if(m_numTargets >= m_maxTargets) return false;
		m_textures[m_numTargets] = texture;
		m_texturesCached[m_numTargets] = true;

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_numTargets, target, texture->textureHandle, level);
		m_numTargets++;
		resetDrawBuffers();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return true;
	}

	bool RenderTarget::initialize()
	{
		// Create FBO
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		
		// Create G-Buffer textures
		uint32_t textures[m_maxTargets];
		glGenTextures(m_numTargets, textures);

		for(uint32_t i = 0; i < m_numTargets; i++)
		{
			m_textures[i]->textureHandle = textures[i];
			glBindTexture(GL_TEXTURE_2D, m_textures[i]->textureHandle);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->textureHandle, 0);
			m_textures[i].setLoaded();
		}

		resetDrawBuffers();

		if(m_numTargets > 0)
		{
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(status != GL_FRAMEBUFFER_COMPLETE)
			{
				Logger::log("vul::RenderTarget::initialize: Framebuffer error (0x%x)", status);
				release();
				return false;
			}
		}

		// Restore default FBO
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		for(uint32_t i = 0; i < m_maxTargets; i++) m_texturesCached[i] = false;

		return true;
	}

	void RenderTarget::release()
	{
		for(uint32_t i = 0; i < m_numTargets; i++)
		{
			if(!m_texturesCached[i]) glDeleteTextures(1, &m_textures[i]->textureHandle);
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_fbo);
		m_loaded = false;
	}

	void RenderTarget::resetDrawBuffers()
	{
		if(m_numTargets == 0) return;

		GLenum* drawBuffers = new GLenum[m_numTargets];
		for(uint32_t i = 0; i < m_numTargets; i++)
			drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

		glDrawBuffers(m_numTargets, drawBuffers);
		delete[] drawBuffers;
	}
}
