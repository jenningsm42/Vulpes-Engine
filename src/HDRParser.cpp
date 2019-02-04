#define VULPESENGINE_EXPORT

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

#include <GL/glew.h>

#include <vulpes/HDRParser.hpp>

#include "Logger.h"

namespace vul
{
	HDRParser::HDRParser() : m_textureData(nullptr), m_rgbeData(nullptr)
	{
		reset();
	}

	HDRParser::~HDRParser()
	{
		reset();
	}

	bool HDRParser::parse(const uint8_t* buffer, uint32_t size)
	{
		reset();
		m_buffer = buffer;
		m_size = size;

		// TODO: Check size for unexpected eof
		// TODO: Can \r\n be used instead of only \n?
		if(memcmp(m_buffer, "#?RADIANCE\n", 11) != 0)
		{
			Logger::log("vul::HDRParser::parse: Invalid file format");
			return false;
		}

		uint32_t curpos = 11;
		int32_t result = 1;
		while((result = parseHeaderLine()) == 1); // Scan over header lines

		if(result == -1)
		{
			Logger::log("vul::HDRParser::parse: Invalid header format");
			return false;
		}

		if(m_width == 0 || m_height == 0)
		{
			Logger::log("vul::HDRParser::parse: Empty image");
			return false;
		}

		// curpos now points to beginning of first scanline
		m_textureData = new float[3 * m_width * m_height];
		m_rgbeData = new uint8_t[4 * m_width * m_height];

		for(uint32_t y = 0; y < m_height; y++)
		{
			if(!parseScanline(y))
			{
				Logger::log("vul::HDRParser::parse: Unable to parse scanline");
				return false;
			}
		}

		convertToFloat();
		
		return true;
	}

	void HDRParser::getDimensions(uint32_t* width, uint32_t* height)
	{
		*width = m_width;
		*height = m_height;
	}

	bool HDRParser::getTextureData(float* data)
	{
		if(m_textureData)
		{
			memcpy(data, m_textureData, m_width * m_height * 3 * sizeof(float));
			return true;
		}
		return false;
	}

	int32_t HDRParser::parseHeaderLine()
	{
		// TODO: Support full RADIANCE format
		// TODO: Write more robust parser (i.e. sscanf("-Y %ld +X %ld", ...), check lengths)
		switch(m_buffer[m_curpos])
		{
		case 'F': // FORMAT
		{
			if(memcmp(&m_buffer[m_curpos], "FORMAT=", 7) != 0) return -1;
			m_curpos += 7;
			if(memcmp(&m_buffer[m_curpos], "32-bit_rle_rgbe\n", 16) != 0
				&& memcmp(&m_buffer[m_curpos], "32-bit_rle_xyze\n", 16) != 0) return -1;
			m_curpos += 16;
		} return 1;
		case 'E': // EXPOSURE
		{
			if(memcmp(&m_buffer[m_curpos], "EXPOSURE=", 9) != 0) return -1;
			m_curpos += 9;
			std::string exposure;
			while(m_buffer[m_curpos] != '\n') exposure += m_buffer[m_curpos++];
			m_exposure = std::stof(exposure);
		} return 1;
		case '-': // Resolution string (only accepts -Y M +X N for now)
		{
			// Height
			if(memcmp(&m_buffer[m_curpos], "-Y ", 3) != 0) return -1;
			m_curpos += 3;
			std::string y_res;
			while(m_buffer[m_curpos] != '-' && m_buffer[m_curpos] != '+')
				y_res += m_buffer[m_curpos++];
			m_height = std::stoul(y_res);

			// Width
			if(memcmp(&m_buffer[m_curpos], "+X ", 3) != 0) return -1;
			m_curpos += 3;
			std::string x_res;
			while(m_buffer[m_curpos] != '\n') x_res += m_buffer[m_curpos++];
			m_width = std::stoul(x_res);
			m_curpos++;
		} return 0; // Finished parsing header
		case '\n': m_curpos++; return 1;
		case '#': // Comment
		{
			while(m_buffer[m_curpos++] != '\n');
		} return 1;
		default: return -1;
		}
	}

	bool HDRParser::parseScanline(uint32_t y)
	{
		if(memcmp(&m_buffer[m_curpos], "\x2\x2", 2) == 0) // Adaptive RLE
		{
			uint8_t lengthBytes[2];
			lengthBytes[1] = m_buffer[m_curpos + 2]; // Big endian -- TODO: check for big/little endianness on CPU
			lengthBytes[0] = m_buffer[m_curpos + 3];
			uint16_t length = *(uint16_t*)lengthBytes;
			m_curpos += 4;

			for(uint32_t col = 0; col < 4; col++) // All red mantissas first, green second, etc
			{
				for(uint32_t i = 0; i < length;)
				{
					uint8_t code = m_buffer[m_curpos++];
					if(code > 128) // Run
					{
						code &= 127; // Remove 'sign bit'
						uint8_t val = m_buffer[m_curpos++];
						while(code--)
							m_rgbeData[4 * ((i++) + m_width * y) + col] = val;
					}
					else // Just list of components
					{
						while(code--)
							m_rgbeData[4 * ((i++) + m_width * y) + col] = m_buffer[m_curpos++];
					}
				}
			}
			return true;
		}
		// TODO: Support old RLE and uncompressed

		Logger::log("vul::HDRParser::parseScanline: Only adaptive RLE is supported");
		return false;
	}

	void HDRParser::convertToFloat()
	{
		uint32_t length = m_width * m_height;
		for(uint32_t i = 0; i < length; i++)
		{
			if(m_rgbeData[4 * i + 3] != 0)
			{
				float f = std::ldexp(1.f, (int8_t)(m_rgbeData[4 * i + 3] - (int8_t)(128+8)));
				m_textureData[3 * i] = m_rgbeData[4 * i] * f * m_exposure;
				m_textureData[3 * i + 1] = m_rgbeData[4 * i + 1] * f * m_exposure;
				m_textureData[3 * i + 2] = m_rgbeData[4 * i + 2] * f * m_exposure;
			}
			else
			{
				m_textureData[3 * i] = m_textureData[3 * i + 1] = m_textureData[3 * i + 2] = 0.f;
			}
		}
	}

	void HDRParser::reset()
	{
		m_buffer = nullptr;
		m_size = 0;
		m_curpos = 0;

		m_exposure = 1.f;
		m_width = 0;
		m_height = 0;
		if(m_textureData)
		{
			delete[] m_textureData;
			m_textureData = nullptr;
		}
		if(m_rgbeData)
		{
			delete[] m_rgbeData;
			m_rgbeData = nullptr;
		}
	}
}
