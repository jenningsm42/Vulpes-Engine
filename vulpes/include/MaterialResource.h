#ifndef _VUL_MATERIALRESOURCE_H
#define _VUL_MATERIALRESOURCE_H
#include "Resource.h"
#include <cstdint>

namespace vul
{
	struct MaterialResource : public Resource
	{
		uint32_t programHandle = 0;
	};
}

#endif // _VUL_MATERIALRESOURCE_H