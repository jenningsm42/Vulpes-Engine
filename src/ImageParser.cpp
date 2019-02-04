#define VULPESENGINE_EXPORT

#include <cstring>

#include <GL/glew.h>

#include <vulpes/ImageParser.hpp>

#include "Logger.h"

namespace vul
{
	ImageParser::ImageParser()
	{
		m_info.loaded = false;
	}

	ImageParser::~ImageParser()
	{
	}

	bool ImageParser::parse(const uint8_t* buffer, uint32_t size)
	{
		m_info.loaded = false;
		if(size >= 4 && memcmp(buffer, "DDS ", 4) == 0) // DDS
		{
			// TODO: Update DDSParser
			if(!m_parserDDS.parse(buffer, size))
				return false;

			m_parserDDS.getDimensions(&m_info.width, &m_info.height);
			m_info.s3tc = true;
			m_info.internalFormat = m_parserDDS.getInternalFormat();
			m_info.format = GL_RGBA;
			m_info.channelType = GL_UNSIGNED_BYTE;
			m_info.numChannels = 4;
			m_info.numMipMaps = m_parserDDS.getMipMapCount();
			m_info.imageType = ImageType::DDS;
			m_info.loaded = true;

		}
		else if(size >= 11 && memcmp(buffer, "#?RADIANCE\n", 11) == 0) // HDR, PIC
		{
			if(!m_parserHDR.parse(buffer, size))
				return false;

			m_parserHDR.getDimensions(&m_info.width, &m_info.height);
			m_info.s3tc = false;
			m_info.internalFormat = GL_RGB32F;
			m_info.format = GL_RGB;
			m_info.channelType = GL_FLOAT;
			m_info.numChannels = 3;
			m_info.numMipMaps = 1;
			m_info.imageType = ImageType::HDR;
			m_info.loaded = true;
		}
		else
			return false;

		return true;
	}

	ImageInfo ImageParser::getImageInfo()
	{
		return m_info;
	}

	uint32_t ImageParser::getSize(uint32_t level)
	{
		switch(m_info.imageType)
		{
		case ImageType::HDR: return m_info.width * m_info.height * m_info.numChannels;
		case ImageType::DDS: return m_parserDDS.getSize(level);
		default: return 0;
		}
	}

	bool ImageParser::getFloatImage(float* data, uint32_t mipmap)
	{
		switch(m_info.imageType)
		{
		case ImageType::HDR: return m_parserHDR.getTextureData(data);
		default: return false;
		}
	}

	bool ImageParser::getIntImage(uint8_t* data, uint32_t mipmap)
	{
		switch(m_info.imageType)
		{
		case ImageType::DDS: return m_parserDDS.getTextureData(data, mipmap);
		default: return false;
		}
	}
}
