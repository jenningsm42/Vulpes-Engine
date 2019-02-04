#ifndef _VUL_MESHDATA_HPP
#define _VUL_MESHDATA_HPP

#include <cstdint>

namespace vul
{
	struct MeshData
	{
		MeshData() {}
		MeshData(uint32_t vertexCount, uint32_t indexCount,
			bool hasNormals = false, bool hasTB = false,
			bool hasUVCoordinates = false) : vertexCount(vertexCount),
			indexCount(indexCount)
		{
			vertices = new float[vertexCount * 3];
			indices = new uint32_t[indexCount];
			normals = hasNormals ? new float[vertexCount * 3] : nullptr;
			tangents = hasTB ? new float[vertexCount * 3] : nullptr;
			bitangents = hasTB ? new float[vertexCount * 3] : nullptr;
			UVCoordinates = hasUVCoordinates ? new float[vertexCount * 2] : nullptr;
		}

		~MeshData()
		{
			if(vertices) delete[] vertices;
			if(indices) delete[] indices;
			if(normals) delete[] normals;
			if(UVCoordinates) delete[] UVCoordinates;
			if(tangents) delete[] tangents;
			if(bitangents) delete[] bitangents;
		}

		void initialize(uint32_t vertexCount, uint32_t indexCount,
			bool hasNormals = false, bool hasTB = false,
			bool hasUVCoordinates = false)
		{
			this->vertexCount = vertexCount;
			this->indexCount = indexCount;
			vertices = new float[vertexCount * 3];
			indices = new uint32_t[indexCount];
			normals = hasNormals ? new float[vertexCount * 3] : nullptr;
			tangents = hasTB ? new float[vertexCount * 3] : nullptr;
			bitangents = hasTB ? new float[vertexCount * 3] : nullptr;
			UVCoordinates = hasUVCoordinates ? new float[vertexCount * 2] : nullptr;
		}

		MeshData& operator=(const MeshData& rhs)
		{
			vertices = rhs.vertices;
			indices = rhs.indices;
			normals = rhs.normals;
			tangents = rhs.tangents;
			bitangents = rhs.bitangents;
			UVCoordinates = rhs.UVCoordinates;
			return *this;
		}

		float* vertices;
		uint32_t* indices;
		float* normals;
		float* tangents;
		float* bitangents;
		float* UVCoordinates;
		uint32_t vertexCount;
		uint32_t indexCount;
	};
}

#endif // _VUL_MESHDATA_HPP
