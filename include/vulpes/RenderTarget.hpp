#ifndef _VUL_RENDERTARGET_HPP
#define _VUL_RENDERTARGET_HPP

#include <cstdint>
#include <string>

#include "Export.hpp"
#include "Handle.hpp"
#include "Texture.hpp"

namespace vul
{
	class ResourceCache;

	class VEAPI RenderTarget
	{
	public:
		RenderTarget(uint32_t width, uint32_t height, uint32_t numTargets = 1);
		~RenderTarget();

		void beginWrite();
		void endWrite();

		Handle<Texture> getTexture(uint32_t index = 0);
		bool cacheTexture(const std::string& name, Handle<ResourceCache>, uint32_t index = 0);

		bool addTarget(Handle<Texture>&, uint32_t target, uint32_t level = 0);

	private:
		int32_t m_defaultViewport[4]; // x, y, width, height
		uint32_t m_width, m_height;
		uint32_t m_fbo;
		const static uint32_t m_maxTargets = 8;
		Handle<Texture> m_textures[m_maxTargets];
		bool m_texturesCached[m_maxTargets];
		uint32_t m_numTargets;
		bool m_loaded;

		bool initialize();
		void release();
		void resetDrawBuffers();
	};
}

#endif // _VUL_RENDERTARGET_HPP
