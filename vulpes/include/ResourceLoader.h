#ifndef _VUL_RESOURCELOADER_H
#define _VUL_RESOURCELOADER_H
#include <string>
#include "Export.h"
#include <cstdint>
#include "MeshResource.h"
#include "TextureResource.h"
#include "MaterialResource.h"

namespace vul
{
	class ResourceCache;

	class VEAPI ResourceLoader
	{
	public:
		ResourceLoader();
		~ResourceLoader();

		MeshResource* loadMeshFromFile(const std::string& path);
		MeshResource* loadMeshFromData(float* vertices, uint32_t vertexCount,
			uint32_t* indices, uint32_t indexCount, float* normals = 0, float* uvcoords = 0);

		TextureResource* loadTextureFromFile(const std::string& path);

		MaterialResource* loadMaterialFromFile(const std::string& vsPath, const std::string& fsPath);
		MaterialResource* loadMaterialFromText(const int8_t* vsContent, const int8_t* fsContent);

	private:
		ResourceCache* m_resourceCache;

		inline bool validateShader(uint32_t shaderHandle);
		inline bool validateProgram(uint32_t programHandle);
	};
}

#endif // _VUL_RESOURCELOADER_H