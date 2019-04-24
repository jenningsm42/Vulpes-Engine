#ifndef _VUL_MESH_HPP
#define _VUL_MESH_HPP

#include <cstdint>

#include "Skeleton.hpp"

namespace vul {
    struct Mesh {
        uint32_t vao = 0; // Handle to vertex array object
        uint32_t ib = 0; // Handle to index buffer
        uint32_t ic = 0; // Index count
        BoneNameToIndexMap boneNameToIndex;
    };
}

#endif // _VUL_MESH_HPP
