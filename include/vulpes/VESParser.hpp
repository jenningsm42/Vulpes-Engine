#ifndef _VUL_VESPARSER_H
#define _VUL_VESPARSER_H

#include "Export.hpp"
#include "Handle.hpp"
#include "Skeleton.hpp"

namespace vul {
    class VEAPI VESParser {
    public:
        bool parse(Handle<Skeleton>&, const uint8_t* buffer, uint32_t size);
    };
}

#endif
