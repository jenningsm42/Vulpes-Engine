#ifndef _VUL_TEXTURERESOURCE_H
#define _VUL_TEXTURERESOURCE_H
#include "Resource.h"
#include <cstdint>

namespace vul
{
	struct TextureResource : public Resource
	{
		uint32_t textureHandle = 0;
	};
}

#endif // _VUL_TEXTURERESOURCE_H