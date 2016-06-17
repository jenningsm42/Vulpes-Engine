#ifndef _VUL_FORWARDRENDERER_H
#define _VUL_FORWARDRENDERER_H
#include "Export.h"
#include "Renderer.h"
#include "Material.h"
#include "Handle.h"

namespace vul
{
	class VEAPI ForwardRenderer : public Renderer
	{
	public:
		ForwardRenderer();
		~ForwardRenderer();

		void render() override;

		void setWireframeMode(bool) override;

	private:
		Handle<Material> m_shader;
	};
}

#endif // _VUL_FORWARDRENDERER_H