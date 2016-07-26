#ifndef _VUL_RESOURCELOADER_H
#define _VUL_RESOURCELOADER_H
#include <string>
#include "Export.h"
#include <cstdint>
#include "ResourceCache.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

namespace vul
{
	struct MeshData
	{
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

		float* vertices;
		uint32_t* indices;
		float* normals;
		float* tangents;
		float* bitangents;
		float* UVCoordinates;
		uint32_t vertexCount;
		uint32_t indexCount;
	};

	class VEAPI ResourceLoader
	{
	public:
		ResourceLoader();
		~ResourceLoader();

		Handle<Mesh> loadMeshFromFile(const std::string& path);
		Handle<Mesh> loadMeshFromData(const MeshData&);

		Handle<Texture> loadTextureFromFile(const std::string& path);
		Handle<Texture> loadTextureFromColor(float red, float green, float blue);

		Handle<Mesh> getPlane();

	private:
		ResourceCache m_resourceCache;

		void createPlane();
	};
}

#endif // _VUL_RESOURCELOADER_H