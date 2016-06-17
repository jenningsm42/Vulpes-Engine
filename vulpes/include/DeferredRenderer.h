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
		// Geometry shader
		ViewMatrix = 0,
		ProjectionMatrix = 4,
		ModelMatrix = 8,
		Near = 12,
		ReflectionCoefficient = 13,
		Roughness = 14,
		ColorMap = 15
	};

	enum struct DeferredLightUniformLocations
	{
		ColorBuffer = 0,
		NormalBuffer = 1,
		DepthBuffer = 2,
		MiscBuffer = 3,
		Near = 4,
		Far = 5,
		LightArray = 6
	};

	class VEAPI DeferredRenderer : public Renderer
	{
	public:
		DeferredRenderer();
		~DeferredRenderer();

		void setActiveCamera(Camera&) override;

		void render() override;

		void setWireframeMode(bool) override;
		void setClearColor(float r, float g, float b, float a = 1.f);

	private:
		GBuffer m_gbuffer;
		Handle<Material> m_geometryShader;
		Handle<Material> m_lightShader;
		Mesh m_quadMesh;
		float m_color[4];
		bool m_wireframe;

		void geometryPass();
		void lightPass();

		void createQuad();
	};
}

#endif // _VUL_DEFERREDRENDERER_H