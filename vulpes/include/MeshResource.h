#ifndef _VUL_MESHRESOURCE_H
#define _VUL_MESHRESOURCE_H
#include "Resource.h"
#include <cstdint>

namespace vul
{
	struct MeshResource : public Resource
	{
		uint32_t vao = 0; // Handle to vertex array object
		uint32_t ib = 0; // Handle to index buffer
		uint32_t ic = 0; // Index count
	};
}

#endif // _VUL_MESHRESOURCE_H