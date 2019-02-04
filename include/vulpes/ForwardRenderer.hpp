#ifndef _VUL_FORWARDRENDERER_HPP
#define _VUL_FORWARDRENDERER_HPP

#include "Export.hpp"
#include "Handle.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"

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
		Handle<Shader> m_shader;
	};
}

#endif // _VUL_FORWARDRENDERER_HPP
