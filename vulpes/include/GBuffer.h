#ifndef _VUL_GBUFFER_H
#define _VUL_GBUFFER_H
#include <cstdint>
#include "Export.h"

namespace vul
{
	class VEAPI GBuffer
	{
	public:
		GBuffer(uint32_t textureCount = 8);
		~GBuffer();

		bool initialize(uint32_t windowWidth, uint32_t windowHeight, uint32_t scalingFactor = 1);
		void release();

		void bindWrite();
		void endWrite();

		void bindRead();
		void setReadBuffer(uint32_t index);

		void bindTexture(uint32_t index);
		
	private:
		uint32_t m_fbo; // Frame buffer object
		const static uint32_t m_maxTextures = 8;
		uint32_t m_textureCount;
		uint32_t m_textures[m_maxTextures];
		uint32_t m_depthTexture;
		uint32_t m_windowWidth, m_windowHeight;
		uint32_t m_scalingFactor;
		bool m_loaded;
	};
}

#endif // _VUL_GBUFFER_H