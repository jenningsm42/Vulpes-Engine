#ifndef _VUL_MESHDATA_HPP
#define _VUL_MESHDATA_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Skeleton.hpp"

namespace vul {
    struct MeshData {
        void resize(
            uint32_t vertexCount,
            uint32_t indexCount,
            bool hasNormals = false,
            bool hasTB = false,
            bool hasUVCoordinates = false,
            bool hasBones = false
        ) {
            vertices.resize(vertexCount * 3);
            indices.resize(indexCount);

            if (hasNormals) {
                normals.resize(vertexCount * 3);
            }

            if (hasTB) {
                tangents.resize(vertexCount * 3);
                bitangents.resize(vertexCount * 3);
            }

            if (hasUVCoordinates) {
                UVCoordinates.resize(vertexCount * 2);
            }

            if (hasBones) {
                vertexWeights.resize(vertexCount * 4);
                vertexBones.resize(vertexCount * 4);
            }
        }

        MeshData& operator=(const MeshData& rhs) {
            vertices = rhs.vertices;
            indices = rhs.indices;
            normals = rhs.normals;
            tangents = rhs.tangents;
            bitangents = rhs.bitangents;
            UVCoordinates = rhs.UVCoordinates;
            return *this;
        }

        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        std::vector<float> normals;
        std::vector<float> tangents;
        std::vector<float> bitangents;
        std::vector<float> UVCoordinates;
        std::vector<float> vertexWeights;
        std::vector<uint8_t> vertexBones;
        BoneNameToIndexMap boneNameToIndex;
    };
}

#endif // _VUL_MESHDATA_HPP
