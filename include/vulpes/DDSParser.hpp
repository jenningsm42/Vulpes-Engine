#ifndef _VUL_DDSPARSER_HPP
#define _VUL_DDSPARSER_HPP

#include "Export.hpp"
#include "Handle.hpp"
#include "Texture.hpp"

namespace vul
{
	class VEAPI DDSParser
	{
	public:
		DDSParser();
		~DDSParser();

		bool parse(const uint8_t* buffer, uint32_t size);

		void getDimensions(uint32_t* width, uint32_t* height);
		uint32_t getInternalFormat();
		uint32_t getMipMapCount();
		uint32_t getSize(uint32_t level);
		bool getTextureData(uint8_t*, uint32_t level);

	private:
		const uint8_t* m_buffer;
		uint32_t m_size;
		uint32_t m_curpos;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_format;
		uint32_t m_numMipMaps;
		uint32_t m_blockSize;
		uint8_t* m_textureData;

		void reset();
	};
}

#endif // _VUL_DDSPARSER_HPP
