#define VULPESENGINE_EXPORT

#include <GL/glew.h>

#include <vulpes/Camera.hpp>
#include <vulpes/ForwardRenderer.hpp>
#include <vulpes/Scene.hpp>

#include "Logger.h"

namespace vul
{
	ForwardRenderer::ForwardRenderer()
	{
		m_error = true;
		Logger::log("vul::ForwardRenderer::ForwardRenderer: Forward rendering not supported at this time");
	}

	ForwardRenderer::~ForwardRenderer()
	{
	}

	void ForwardRenderer::render()
	{
		if(m_error) return;

		if(!m_shader.isLoaded())
		{
			Logger::log("vul::ForwardRenderer::render: Forward PBR shader not loaded");
			m_error = true;
			return;
		}

		if(!m_scene)
		{
			Logger::log("vul::ForwardRenderer::render: No scene assigned to renderer");
			m_error = true;
			return;
		}

		if(!m_camera)
		{
			Logger::log("vul::ForwardRenderer::render: No active camera assigned to renderer");
			m_error = true;
			return;
		}

		glUseProgram(m_shader->programHandle);

		// Global transformations
		uint32_t location = glGetUniformLocation(m_shader->programHandle, "viewMat");
		glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->getViewMatrix()[0][0]);
		location = glGetUniformLocation(m_shader->programHandle, "projMat");
		glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->getProjMatrix()[0][0]);

		uint32_t tmpPolycount = 0;
		for(uint32_t i = 0; i < m_scene->getSceneObjectCount(SceneObjectType::Renderable); i++)
		{
			Handle<RenderableObject> currentObject = m_scene->getRenderableObjectByIndex(i);
			Handle<Mesh> mesh = currentObject->getMesh();
			Handle<Texture> colorMap = currentObject->getColorMap();
			tmpPolycount += mesh->ic / 3;

			// Local transformations
			glm::mat4 modelMatrix = currentObject->getTransformation().getTransformationMatrix();
			glm::mat3 normalMatrix = currentObject->getTransformation().getNormalMatrix();

			location = glGetUniformLocation(m_shader->programHandle, "modelMat");
			glUniformMatrix4fv(location, 1, GL_FALSE, &modelMatrix[0][0]);
			location = glGetUniformLocation(m_shader->programHandle, "normalMat");
			glUniformMatrix3fv(location, 1, GL_FALSE, &normalMatrix[0][0]);

			// Textures
			if(colorMap->textureHandle != 0)
			{
				location = glGetUniformLocation(m_shader->programHandle, "tex");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, colorMap->textureHandle);

				glUniform1i(location, 0);
			}

			// Extra
			location = glGetUniformLocation(m_shader->programHandle, "near");
			glUniform1f(location, m_camera->getNear());

			location = glGetUniformLocation(m_shader->programHandle, "far");
			glUniform1f(location, m_camera->getFar());

			// Meshes
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
			glDrawElements(GL_TRIANGLES, mesh->ic, GL_UNSIGNED_INT, nullptr);
		}

		m_polycount = tmpPolycount;
	}

	void ForwardRenderer::setWireframeMode(bool wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
	}
}
