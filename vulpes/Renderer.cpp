#define VULPESENGINE_EXPORT
#include "include/Renderer.h"
#include "include/Scene.h"
#include "include/Camera.h"
#include "Logger.h"
#include <gl/glew.h>

namespace vul
{
	Renderer::Renderer() :m_scene(0), m_camera(0), m_polycount(0)
	{
		glUseProgram(0);
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::setScene(Scene* scene)
	{
		m_scene = scene;
	}

	void Renderer::setActiveCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Renderer::render()
	{
		if(!m_scene)
		{
			Logger::log("vul::Renderer::render: No scene assigned to renderer");
			return;
		}

		if(!m_camera)
		{
			Logger::log("vul::Renderer::render: No active camera assigned to renderer");
			return;
		}

		uint32_t tmpPolycount = 0;
		for(int i = 0; i < m_scene->getMaterialsSize(); i++)
		{
			Material* material = m_scene->getMaterial(i);
			uint32_t programHandle = material->getMaterialResource()->programHandle;

			glUseProgram(programHandle);

			handleUniforms(material);

			for(int j = 0; j < material->getRenderablesSize(); j++)
			{
				RenderableObject* cur = material->getRenderableFromIndex(j);
				tmpPolycount += cur->getMeshResource().ic / 3;
				
				// Transformations
				uint32_t location = glGetUniformLocation(programHandle, "viewMat");
				glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->getViewMatrix()[0][0]);
				location = glGetUniformLocation(programHandle, "projMat");
				glUniformMatrix4fv(location, 1, GL_FALSE, &m_camera->getProjMatrix()[0][0]);

				glm::mat4 modelMatrix = cur->getTransformation().getTransformationMatrix();
				glm::mat3 normalMatrix = cur->getTransformation().getNormalMatrix();

				location = glGetUniformLocation(programHandle, "modelMat");
				glUniformMatrix4fv(location, 1, GL_FALSE, &modelMatrix[0][0]);
				location = glGetUniformLocation(programHandle, "normalMat");
				glUniformMatrix3fv(location, 1, GL_FALSE, &normalMatrix[0][0]);

				// Textures
				if(cur->getTextureResource().textureHandle != 0)
				{
					location = glGetUniformLocation(programHandle, "tex");
					glUniform1i(location, 0);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, cur->getTextureResource().textureHandle);
				}

				// Meshes
				MeshResource mr = cur->getMeshResource();
				glBindVertexArray(mr.vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mr.ib);
				glDrawElements(GL_TRIANGLES, mr.ic, GL_UNSIGNED_INT, 0);
			}

			glUseProgram(0);
		}

		m_polycount = tmpPolycount;
	}

	void Renderer::setWireframeMode(bool wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
	}

	uint32_t Renderer::getPolygonCount()
	{
		return m_polycount;
	}

	void Renderer::handleUniforms(Material* material)
	{
		uint32_t programHandle = material->getMaterialResource()->programHandle;
		for(int32_t i = 0; i < material->getSize(); i++)
		{
			MaterialUniform* uniform = material->getUniform(i);
			uint32_t location = glGetUniformLocation(programHandle, uniform->name);

			switch(uniform->type)
			{
			case MaterialUniformType::Float1: glUniform1f(location, uniform->value.float1); break;
			case MaterialUniformType::Float2: glUniform2f(location, uniform->value.float2[0], uniform->value.float2[1]); break;
			case MaterialUniformType::Float3: glUniform3f(location, uniform->value.float3[0], uniform->value.float3[1], uniform->value.float3[2]); break;
			case MaterialUniformType::Float4: glUniform4f(location, uniform->value.float4[0], uniform->value.float4[1], uniform->value.float4[2], uniform->value.float4[3]); break;
			case MaterialUniformType::Float2x2: glUniformMatrix2fv(location, 1, GL_FALSE, &uniform->value.float2x2[0][0]); break;
			case MaterialUniformType::Float3x3: glUniformMatrix3fv(location, 1, GL_FALSE, &uniform->value.float3x3[0][0]); break;
			case MaterialUniformType::Float4x4: glUniformMatrix4fv(location, 1, GL_FALSE, &uniform->value.float4x4[0][0]); break;
			case MaterialUniformType::Int1: glUniform1i(location, uniform->value.int1); break;
			case MaterialUniformType::Int2: glUniform2i(location, uniform->value.int2[0], uniform->value.int2[1]); break;
			case MaterialUniformType::Int3: glUniform3i(location, uniform->value.int3[0], uniform->value.int3[1], uniform->value.int3[2]); break;
			case MaterialUniformType::Int4: glUniform4i(location, uniform->value.int4[0], uniform->value.int4[1], uniform->value.int4[2], uniform->value.int4[3]); break;
			}
		}
	}
}