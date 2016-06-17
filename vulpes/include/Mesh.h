#ifndef _VUL_MESH_H
#define _VUL_MESH_H
#include <cstdint>

namespace vul
{
	struct Mesh
	{
		uint32_t vao = 0; // Handle to vertex array object
		uint32_t ib = 0; // Handle to index buffer
		uint32_t ic = 0; // Index count
	};
}

#endif // _VUL_MESH_H