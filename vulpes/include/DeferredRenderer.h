#ifndef _VUL_DEFERREDRENDERER_H
#define _VUL_DEFERREDRENDERER_H
#include "Export.h"
#include "Renderer.h"
#include "Material.h"
#include "GBuffer.h"
#include "Handle.h"
#include "Mesh.h"

namespace vul
{
	enum struct DeferredGeometryUniformLocations
	{
		ViewMatrix = 0,
		ProjectionMatrix = 4,
		ModelMatrix = 8,
		Near = 12,
		ReflectionCoefficient = 13,
		ColorMap = 14,
		NormalMap = 15,
		RoughnessMap = 16
	};

	enum struct DeferredLightUniformLocations
	{
		ColorBuffer = 0,
		NormalBuffer = 1,
		DepthBuffer = 2,
		MiscBuffer = 3,
		Near = 4,
		Far = 5,
		InverseViewMatrix = 6,
		EnvironmentMap = 9,
		LightArray = 10
	};

	class VEAPI DeferredRenderer : public Renderer
	{
	public:
		DeferredRenderer();
		~DeferredRenderer();

		void setActiveCamera(Camera&) override;
		void setActiveEnvironment(Handle<Texture>);

		void render() override;

		void setWireframeMode(bool) override;
		void setClearColor(float r, float g, float b, float a = 1.f);

	private:
		GBuffer m_gbuffer;
		Handle<Material> m_geometryShader;
		Handle<Material> m_lightShader;
		Handle<Texture> m_environmentMap;
		Mesh m_quadMesh;
		float m_color[4];
		bool m_wireframe;

		void geometryPass();
		void lightPass();

		void createQuad();
	};
}

#endif // _VUL_DEFERREDRENDERER_H