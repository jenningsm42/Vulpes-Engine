#ifndef _VUL_CUSTOMRENDERER_H
#define _VUL_CUSTOMRENDERER_H
#include "Export.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "ResourceLoader.h"

namespace vul
{
	class VEAPI CustomRenderer : public Renderer
	{
	public:
		CustomRenderer(ResourceLoader&);
		~CustomRenderer();

		void render() override;
		void render(RenderTarget* renderTarget);

		void setShader(Handle<Shader>&);
		void copyUniformBufferObject(void* data, uint32_t size);
		void setTexture(Handle<Texture>&, uint32_t index);
		void setCubeMap(Handle<Texture>&, uint32_t index);

		void setWireframeMode(bool) override;
		void setClearColor(float r, float g, float b, float a = 1.f);

	private:
		float m_color[4];
		uint32_t m_ubo;
		bool m_uboLoaded;
		Handle<Mesh> m_quad;
		Handle<Shader> m_shader;
	};
}

#endif // _VUL_CUSTOMRENDERER_H