#define VULPESENGINE_EXPORT
#include "include/DeferredRenderer.h"
#include "include/Scene.h"
#include "include/Camera.h"
#include "include/Mesh.h"
#include "InternalResourceLoader.h"
#include "Logger.h"
#include <gl/glew.h>

namespace vul
{
	DeferredRenderer::DeferredRenderer() : m_gbuffer(4), m_wireframe(false)
	{
		m_geometryShader = InternalResourceLoader::loadMaterialFromFile("data/geometryPass.vs", "data/geometryPass.fs");
		if(!m_geometryShader.isLoaded())
		{
			Logger::log("vul::DeferredRenderer::DeferredRenderer: Unable to open geometry shader");
			m_error = true;
		}

		m_lightShader = InternalResourceLoader::loadMaterialFromFile("data/lightPass.vs", "data/lightPass.fs");
		if(!m_lightShader.isLoaded())
		{
			Logger::log("vul::DeferredRenderer::DeferredRenderer: Unable to open light shader");
			m_error = true;
		}

		for(int i = 0; i < 4; i++) m_color[i] = 1.f;
	}

	DeferredRenderer::~DeferredRenderer()
	{
	}

	void DeferredRenderer::setActiveCamera(Camera& camera)
	{
		m_camera = &camera;
		createQuad();
		m_gbuffer.initialize(camera.getWidth(), camera.getHeight());
	}

	void DeferredRenderer::render()
	{
		if(m_error) return;

		if(!m_geometryShader.isLoaded())
		{
			Logger::log("vul::DeferredRenderer::render: Geometry shader not loaded");
			m_error = true;
			return;
		}

		if(!m_lightShader.isLoaded())
		{
			Logger::log("vul::DeferredRenderer::render: Light shader not loaded");
			m_error = true;
			return;
		}

		if(!m_scene)
		{
			Logger::log("vul::DeferredRenderer::render: No scene assigned to renderer");
			m_error = true;
			return;
		}

		if(!m_camera)
		{
			Logger::log("vul::DeferredRenderer::render: No active camera assigned to renderer");
			m_error = true;
			return;
		}

		geometryPass();
		lightPass();
	}

	void DeferredRenderer::setWireframeMode(bool wireframe)
	{
		m_wireframe = wireframe;
	}

	inline void DeferredRenderer::setClearColor(float r, float g, float b, float a)
	{
		m_color[0] = r;
		m_color[1] = g;
		m_color[2] = b;
		m_color[3] = a;
	}

	void DeferredRenderer::geometryPass()
	{
		// Here, all the data needed to compute the final scene is written into the g-buffer
		// This includes diffuse information, surface normals, UV (texture) coordinates, etc.
		// More can be added, such as a stencil map for special materials, specular maps, and so on

		m_gbuffer.bindWrite();
		
		glDepthMask(GL_TRUE);
		glClearColor(m_color[0], m_color[1], m_color[2], m_color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		if(m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glUseProgram(m_geometryShader->programHandle);

		// Global transformations
		glUniformMatrix4fv(static_cast<GLint>(DeferredGeometryUniformLocations::ViewMatrix), 1, GL_FALSE, &m_camera->getViewMatrix()[0][0]);
		glUniformMatrix4fv(static_cast<GLint>(DeferredGeometryUniformLocations::ProjectionMatrix), 1, GL_FALSE, &m_camera->getProjMatrix()[0][0]);

		uint32_t tmpPolycount = 0;
		for(uint32_t i = 0; i < m_scene->getSceneObjectCount(SceneObjectType::Renderable); i++)
		{
			Handle<RenderableObject> currentObject = m_scene->getRenderableObjectByIndex(i);
			Handle<Mesh> mesh = currentObject->getMesh();
			Handle<Texture> colorMap = currentObject->getColorMap();
			Handle<Texture> normalMap = currentObject->getNormalMap();
			Handle<Texture> roughnessMap = currentObject->getRoughnessMap();
			tmpPolycount += mesh->ic / 3;

			// Local transformations
			glm::mat4 modelMatrix = currentObject->getTransformation().getTransformationMatrix();
			glm::mat3 normalMatrix = currentObject->getTransformation().getNormalMatrix();

			glUniformMatrix4fv(static_cast<GLint>(DeferredGeometryUniformLocations::ModelMatrix), 1, GL_FALSE, &modelMatrix[0][0]);

			// Textures
			if(colorMap->textureHandle != 0)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, colorMap->textureHandle);

				glUniform1i(static_cast<GLint>(DeferredGeometryUniformLocations::ColorMap), 0);
			}

			if(normalMap->textureHandle != 0)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, normalMap->textureHandle);

				glUniform1i(static_cast<GLint>(DeferredGeometryUniformLocations::NormalMap), 1);
			}

			if(roughnessMap->textureHandle != 0)
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, roughnessMap->textureHandle);

				glUniform1i(static_cast<GLint>(DeferredGeometryUniformLocations::RoughnessMap), 2);
			}

			// Extra
			glUniform1f(static_cast<GLint>(DeferredGeometryUniformLocations::Near), m_camera->getNear());
			glUniform1f(static_cast<GLint>(DeferredGeometryUniformLocations::ReflectionCoefficient), currentObject->getReflectionCoefficient());

			// Meshes
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
			glDrawElements(GL_TRIANGLES, mesh->ic, GL_UNSIGNED_INT, nullptr);
		}

		m_gbuffer.endWrite();
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		if(m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		m_polycount = tmpPolycount;
	}

	void DeferredRenderer::lightPass()
	{
		// The g-buffer has been populated with data, now it must be processed into
		// the final scene, then rendered on the screen

		glUseProgram(m_lightShader->programHandle);

		// Pass the gbuffer
		glActiveTexture(GL_TEXTURE0);
		m_gbuffer.bindTexture(0);
		glUniform1i(static_cast<GLint>(DeferredLightUniformLocations::ColorBuffer), 0);

		glActiveTexture(GL_TEXTURE1);
		m_gbuffer.bindTexture(1);
		glUniform1i(static_cast<GLint>(DeferredLightUniformLocations::NormalBuffer), 1);

		glActiveTexture(GL_TEXTURE2);
		m_gbuffer.bindTexture(2);
		glUniform1i(static_cast<GLint>(DeferredLightUniformLocations::DepthBuffer), 2);

		glActiveTexture(GL_TEXTURE3);
		m_gbuffer.bindTexture(3);
		glUniform1i(static_cast<GLint>(DeferredLightUniformLocations::MiscBuffer), 3);

		glUniform1f(static_cast<GLint>(DeferredLightUniformLocations::Near), m_camera->getNear());
		glUniform1f(static_cast<GLint>(DeferredLightUniformLocations::Far), m_camera->getFar());

		// Lights
		for(uint32_t i = 0; i < m_scene->getSceneObjectCount(SceneObjectType::PointLight); i++)
		{
			Handle<PointLight> hLight = m_scene->getPointLightByIndex(i);
			glm::vec4 lightPosViewSpace = m_camera->getViewMatrix() * glm::vec4(hLight->getTransformation().getPosition(), 1.f);
			glm::vec3 lightPosTransformed = glm::vec3(lightPosViewSpace) / lightPosViewSpace.w;

			glUniform3fv(static_cast<GLint>(DeferredLightUniformLocations::LightArray) + i * 3,
				1, &lightPosTransformed[0]);
			glUniform3fv(static_cast<GLint>(DeferredLightUniformLocations::LightArray) + i * 3 + 1,
				1, &hLight->getColor()[0]);
			glUniform1f(static_cast<GLint>(DeferredLightUniformLocations::LightArray) + i * 3 + 2,
				hLight->getIntensity());
		}

		glBindVertexArray(m_quadMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadMesh.ib);
		glDrawElements(GL_TRIANGLES, m_quadMesh.ic, GL_UNSIGNED_INT, nullptr);
	}

	void DeferredRenderer::createQuad()
	{
		float vertices[] = {-1.f, -1.f, 0.f,
							1.f, -1.f, 0.f,
							-1.f, 1.f, 0.f,
							1.f, 1.f, 0.f};

		uint32_t indices[] = {0, 1, 2, 2, 1, 3};

		float uvcoords[] = {0.f, 0.f,
							1.f, 0.f,
							0.f, 1.f,
							1.f, 1.f};

		glm::vec4 homogenousCorners[] = {
			glm::vec4(-1.f, -1.f, 0.f, 1.f),
			glm::vec4(1.f, -1.f, 0.f, 1.f),
			glm::vec4(-1.f, 1.f, 0.f, 1.f),
			glm::vec4(1.f, 1.f, 0.f, 1.f)
		};

		glm::mat4 invProjMat = glm::inverse(m_camera->getProjMatrix());

		float rays[4 * 3];

		for(int i = 0; i < 4; i++)
		{
			glm::vec4 ray = invProjMat * homogenousCorners[i];
			ray /= ray.w;
			ray /= ray.z; // Z-normalize
			ray *= -m_camera->getFar(); // Scale to far plane

			rays[3 * i] = ray.x;
			rays[3 * i + 1] = ray.y;
			rays[3 * i + 2] = ray.z;
		}

		m_quadMesh.ic = 6;
		uint32_t vbo[3];

		glGenVertexArrays(1, &m_quadMesh.vao);
		glBindVertexArray(m_quadMesh.vao);

		glGenBuffers(3, vbo);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// UV coordinates
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), uvcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// Frustum rays
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), rays, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		glGenBuffers(1, &m_quadMesh.ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadMesh.ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
}