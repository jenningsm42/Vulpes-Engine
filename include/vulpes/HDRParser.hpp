#ifndef _VUL_HPPDRPARSER_HPP
#define _VUL_HPPDRPARSER_HPP

#include "Export.hpp"
#include "Handle.hpp"
#include "Texture.hpp"

namespace vul {
    class VEAPI HDRParser {
    public:
        HDRParser();
        ~HDRParser();

        bool parse(const uint8_t* buffer, uint32_t size);

        void getDimensions(uint32_t* width, uint32_t* height);
        bool getTextureData(float*);

    private:
        const uint8_t* m_buffer;
        uint32_t m_size;
        uint32_t m_curpos;
        uint32_t m_width;
        uint32_t m_height;
        float m_exposure;
        float* m_textureData;
        uint8_t* m_rgbeData;

        int32_t parseHeaderLine();
        bool parseScanline(uint32_t y);
        void convertToFloat();
        void reset();
    };
}

#endif // _VUL_HPPDRPARSER_HPP
