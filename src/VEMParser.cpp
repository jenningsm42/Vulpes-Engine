#define VULPESENGINE_EXPORT

#include <cstring>

#include <vulpes/VEMParser.hpp>

#include "Logger.h"


namespace vul {
    VEMParser::VEMParser() {
    }

    VEMParser::~VEMParser() {
    }

    bool VEMParser::parse(MeshData* meshData, const uint8_t* buffer, uint32_t size) {
        /* VULPES ENGINE MESH FORMAT SPECIFICATION
            Header:
                magic(4): int8_t[4] "VULP"
                version(2): uint16_t 0x0005 or 0x0006
                flags(1): uint8_t
                    bit 0: set = has normals
                    bit 1: set = has uvcoords
                    bit 2: set = has tangent and bitangents
                    bit 3: set = has per-vertex weights and bones
                    bits 4-7: reserved
                vertex count(4): uint32_t
                index count(4): uint32_t
            Data:
                vertex coordinates(vertex count): {float, float, float}
                indices(vertex count): {uint32_t}
                normals(vertex count): {float, float, float}
                tangents(vertex count): {float, float, float}
                bitangents(vertex count): {float, float, float}
                uvcoords(vertex count): {float, float}
        */

        if (memcmp(buffer, "VULP", 4) != 0) {
            Logger::log("vul::VEMParser::parse: Invalid mesh format");
            return false;
        }

        uint32_t curpos = 4;
        uint16_t version = *(uint16_t*)&buffer[curpos];
        curpos += 2;
        if (version != 5 && version != 6) {
            Logger::log("vul::VEMParser::parse: Invalid version number");
            return false;
        }

        uint8_t flags = *(uint8_t*)&buffer[curpos++];
        bool hasNormals = (flags & 1) != 0;
        bool hasUVcoords = (flags & 2) != 0;
        bool hasTB = (flags & 4) != 0;
        bool hasWeights = (flags & 8) != 0;

        uint32_t vcount = *(uint32_t*)&buffer[curpos];
        curpos += 4;
        if (vcount == 0) {
            Logger::log("vul::VEMParser::parse: No vertices found");
            return false;
        }

        uint32_t icount = *(uint32_t*)&buffer[curpos];
        curpos += 4;
        if (icount == 0) {
            Logger::log("vul::VEMParser::parse: No indices found");
            return false;
        }

        uint8_t boneCount = 0;
        if (version == 6) {
            boneCount = buffer[curpos++];
        }

        const float* floatBuffer = (const float*)(buffer + curpos);
        meshData->vertices.assign(floatBuffer, floatBuffer + vcount * 3);
        curpos += sizeof(float) * vcount * 3;

        const uint32_t* intBuffer = (const uint32_t*)(buffer + curpos);
        meshData->indices.assign(intBuffer, intBuffer + icount);
        curpos += sizeof(uint32_t) * icount;

        if (hasNormals) {
            floatBuffer = (const float*)(buffer + curpos);
            meshData->normals.assign(floatBuffer, floatBuffer + vcount * 3);
            curpos += sizeof(float) * vcount * 3;
        }

        if (hasTB) {
            floatBuffer = (const float*)(buffer + curpos);
            meshData->tangents.assign(floatBuffer, floatBuffer + vcount * 3);
            curpos += sizeof(float) * vcount * 3;

            floatBuffer = (const float*)(buffer + curpos);
            meshData->bitangents.assign(floatBuffer, floatBuffer + vcount * 3);
            curpos += sizeof(float) * vcount * 3;
        }

        if (hasUVcoords) {
            floatBuffer = (const float*)(buffer + curpos);
            meshData->UVCoordinates.assign(floatBuffer, floatBuffer + vcount * 2);
            curpos += sizeof(float) * vcount * 2;
        }

        if (hasWeights && version == 6) {
            for (uint8_t i = 0; i < boneCount; i++) {
                uint8_t boneIndex = buffer[curpos++];
                uint8_t nameLength = buffer[curpos++];
                std::string name;
                name.assign(&buffer[curpos], &buffer[curpos + nameLength]);
                curpos += nameLength;
                meshData->boneNameToIndex[name] = boneIndex;
            }

            meshData->vertexBones.assign(&buffer[curpos], &buffer[curpos + sizeof(uint8_t) * vcount * 4]);
            curpos += sizeof(uint8_t) * vcount * 4;

            floatBuffer = (const float*)(buffer + curpos);
            meshData->vertexWeights.assign(floatBuffer, floatBuffer + vcount * 4);
            curpos += sizeof(float) * vcount * 4;
        }

        return true;
    }
}
