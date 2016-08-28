#define VULPESENGINE_EXPORT
#include "include/VEMParser.h"
#include "Logger.h"
#include <cstring>

namespace vul
{
	VEMParser::VEMParser()
	{
	}

	VEMParser::~VEMParser()
	{
	}

	bool VEMParser::parse(MeshData* meshData, const uint8_t* buffer, uint32_t size)
	{
		/* VULPES ENGINE MESH FORMAT SPECIFICATION
			Header:
				magic(4): int8_t[4] "VULP"
				version(2): uint16_t 0x0005
				flags(1): uint8_t
					bit 0: set = has normals
					bit 1: set = has uvcoords
					bit 2: set = has tangent and bitangents
					bits 3-7: reserved
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

		if(memcmp(buffer, "VULP", 4) != 0)
		{
			Logger::log("vul::VEMParser::parse: Invalid mesh format");
			return false;
		}

		uint32_t curpos = 4;
		uint16_t version = *(uint16_t*)&buffer[curpos];
		curpos += 2;
		if(version != 0x0005)
		{
			Logger::log("vul::VEMParser::parse: Invalid version number");
			return false;
		}

		uint8_t flags = *(uint8_t*)&buffer[curpos++];
		bool hasNormals = (flags & 1) != 0;
		bool hasUVcoords = (flags & 2) != 0;
		bool hasTB = (flags & 4) != 0;
		
		uint32_t vcount = *(uint32_t*)&buffer[curpos];
		curpos += 4;
		if(vcount == 0)
		{
			Logger::log("vul::VEMParser::parse: No vertices found");
			return false;
		}

		uint32_t icount = *(uint32_t*)&buffer[curpos];
		curpos += 4;
		if(icount == 0)
		{
			Logger::log("vul::VEMParser::parse: No indices found");
			return false;
		}

		meshData->initialize(vcount, icount, hasNormals, hasUVcoords, hasTB);

		memcpy(meshData->vertices, &buffer[curpos], sizeof(float) * meshData->vertexCount * 3);
		curpos += sizeof(float) * meshData->vertexCount * 3;
		memcpy(meshData->indices, &buffer[curpos], sizeof(uint32_t) * meshData->indexCount);
		curpos += sizeof(uint32_t) * meshData->indexCount;
		if(hasNormals)
		{
			memcpy(meshData->normals, &buffer[curpos], sizeof(float) * meshData->vertexCount * 3);
			curpos += sizeof(float) * meshData->vertexCount * 3;
		}
		if(hasTB)
		{
			memcpy(meshData->tangents, &buffer[curpos], sizeof(float) * meshData->vertexCount * 3);
			curpos += sizeof(float) * meshData->vertexCount * 3;
			memcpy(meshData->bitangents, &buffer[curpos], sizeof(float) * meshData->vertexCount * 3);
			curpos += sizeof(float) * meshData->vertexCount * 3;
		}
		if(hasUVcoords) memcpy(meshData->UVCoordinates, &buffer[curpos], sizeof(float) * meshData->vertexCount * 2);

		return true;
	}
}