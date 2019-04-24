#ifndef _VUL_IMAGEPARSER_HPP
#define _VUL_IMAGEPARSER_HPP

#include <cstdint>

#include "DDSParser.hpp"
#include "Export.hpp"
#include "HDRParser.hpp"

namespace vul {
    enum struct ImageType {
        DDS,
        HDR
    };

    struct ImageInfo {
        bool loaded;
        uint32_t width;
        uint32_t height;
        bool s3tc;
        uint32_t internalFormat;
        uint32_t format;
        uint32_t channelType;
        uint32_t numChannels;
        uint32_t numMipMaps;
        ImageType imageType;
    };

    class VEAPI ImageParser {
    public:
        ImageParser();
        ~ImageParser();

        bool parse(const uint8_t* buffer, uint32_t size);

        ImageInfo getImageInfo();
        uint32_t getSize(uint32_t level = 0);
        bool getFloatImage(float* data, uint32_t level = 0);
        bool getIntImage(uint8_t* data, uint32_t level = 0);

    private:
        ImageInfo m_info;
        DDSParser m_parserDDS;
        HDRParser m_parserHDR;
    };
}

#endif // _VUL_IMAGEPARSER_HPP
