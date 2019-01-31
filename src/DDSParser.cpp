#define VULPESENGINE_EXPORT
#include "include/DDSParser.h"
#include "Logger.h"
#include <cstring>
#include <cmath>
#include <GL/glew.h>

#define FOURCC_DXT1 0x31545844 // DXT1
#define FOURCC_DXT3 0x33545844 // DXT3
#define FOURCC_DXT5 0x35545844 // DXT5

namespace vul
{
	DDSParser::DDSParser() : m_textureData(nullptr)
	{
		reset();
	}

	DDSParser::~DDSParser()
	{
		reset();
	}

	bool DDSParser::parse(const uint8_t* buffer, uint32_t size)
	{
		reset();
		m_buffer = buffer;
		m_size = size;

		// TODO: check size for unexpected eof

		if(memcmp(buffer, "DDS ", 4) != 0)
		{
			Logger::log("vul::DDSParser::parse: Invalid file format");
			return false;
		}

		m_height = *(uint32_t*)&buffer[12];
		m_width = *(uint32_t*)&buffer[16];
		uint32_t linearSize = *(uint32_t*)&buffer[20];
		m_numMipMaps = *(uint32_t*)&buffer[28];
		uint32_t fourCC = *(uint32_t*)&buffer[84];

		if(m_width == 0 || m_height == 0)
		{
			Logger::log("vul::DDSParser::parse: Empty image");
			return false;
		}
		
		if(m_numMipMaps == 0) m_numMipMaps = 1;
		else if(m_numMipMaps > 32) m_numMipMaps = 32;

		switch(fourCC)
		{
		case FOURCC_DXT1: m_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
		case FOURCC_DXT3: m_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
		case FOURCC_DXT5: m_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
		default:
			Logger::log("vul::DDSParser::parse: DDS file is not DXT1, DXT3, or DXT5");
			return false;
		}

		m_blockSize = (fourCC == FOURCC_DXT1 ? 8 : 16);

		if(m_width < uint32_t(1) << (m_numMipMaps - 1)) // Mipmap mismatch - see if we can resolve it
		{
			// Only allow power of two in this case
			if(((m_width - 1) & m_width) != 0)
			{
				Logger::log("vul::DDSParser::parse: Mipmap mismatch and non power-of-two width");
				return false;
			}

			// Maximum number of mipmaps allowed based on width
			m_numMipMaps = static_cast<uint32_t>(std::log2(m_width)) + 1;
		}

		if(m_height < uint32_t(1) << (m_numMipMaps - 1)) // Mipmap mismatch - see if we can resolve it
		{
			// Only allow power of two in this case
			if(((m_height - 1) & m_height) != 0)
			{
				Logger::log("vul::DDSParser::parse: Mipmap mismatch and non power-of-two height");
				return false;
			}

			// Maximum number of mipmaps allowed based on height
			m_numMipMaps = static_cast<uint32_t>(std::log2(m_height)) + 1;
		}

		m_textureData = new uint8_t[m_size - 128];
		memcpy(m_textureData, &buffer[128], m_size - 128);

		return true;
	}

	void DDSParser::getDimensions(uint32_t* width, uint32_t* height)
	{
		*width = m_width;
		*height = m_height;
	}

	uint32_t DDSParser::getInternalFormat()
	{
		return m_format;
	}

	uint32_t DDSParser::getMipMapCount()
	{
		return m_numMipMaps;
	}

	uint32_t DDSParser::getSize(uint32_t level)
	{
		uint32_t factor = uint32_t(1) << level;
		return ((m_width / factor + 3) / 4) * ((m_height / factor + 3) / 4) * m_blockSize;
	}
	bool DDSParser::getTextureData(uint8_t* data, uint32_t level)
	{
		if(m_textureData)
		{
			uint32_t offset = 0;
			for(uint32_t i = 0; i < level; i++)
				offset += getSize(i);

			memcpy(data, m_textureData + offset, getSize(level));
			return true;
		}
		return false;
	}

	void DDSParser::reset()
	{
		m_buffer = nullptr;
		m_size = 0;
		m_curpos = 0;

		m_width = 0;
		m_height = 0;
		m_format = GL_RGBA8;
		m_numMipMaps = 0;
		m_blockSize = 0;

		delete[] m_textureData;
		m_textureData = nullptr;
	}
}
