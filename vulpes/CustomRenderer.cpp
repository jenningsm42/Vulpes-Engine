#define VULPESENGINE_EXPORT
#include "include/CustomRenderer.h"
#include <gl/glew.h>

namespace vul
{
	CustomRenderer::CustomRenderer(ResourceLoader& rl) : m_uboLoaded(false)
	{
		m_quad = rl.getQuad();
		for(int i = 0; i < 4; i++) m_color[i] = 1.f;
	}

	CustomRenderer::~CustomRenderer()
	{
	}

	void CustomRenderer::render()
	{
		render(nullptr);
	}

	void CustomRenderer::render(RenderTarget* rt)
	{
		if(!m_shader.isLoaded()) return;
		glUseProgram(m_shader->programHandle);

		glClearColor(m_color[0], m_color[1], m_color[2], m_color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		if(rt) rt->beginWrite();
		glBindVertexArray(m_quad->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad->ib);
		glDrawElements(GL_TRIANGLES, m_quad->ic, GL_UNSIGNED_INT, nullptr);
		if(rt) rt->endWrite();
	}

	void CustomRenderer::setShader(Handle<Shader>& shader)
	{
		m_shader = shader;
	}

	void CustomRenderer::copyUniformBufferObject(void* data, uint32_t size)
	{
		if(!m_shader.isLoaded()) return;

		if(m_uboLoaded)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
			GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
			memcpy(ptr, data, size);
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}
		else
		{
			uint32_t blockIndex = glGetUniformBlockIndex(m_shader->programHandle, "DataBlock");
			glUniformBlockBinding(m_shader->programHandle, blockIndex, 0);

			glGenBuffers(1, &m_ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);

			glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ubo);
			m_uboLoaded = true;
		}
	}

	void CustomRenderer::setTexture(Handle<Texture>& texture, uint32_t index)
	{
		glUseProgram(m_shader->programHandle);
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture->textureHandle);
		glUniform1i(0, index);
	}

	void CustomRenderer::setCubeMap(Handle<Texture>& texture, uint32_t index)
	{
		glUseProgram(m_shader->programHandle);
		glActiveTexture(GL_TEXTURE0 + index + 4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureHandle);
		glUniform1i(4, index + 4);
	}

	void CustomRenderer::setWireframeMode(bool)
	{
	}

	inline void CustomRenderer::setClearColor(float r, float g, float b, float a)
	{
		m_color[0] = r;
		m_color[1] = g;
		m_color[2] = b;
		m_color[3] = a;
	}
}