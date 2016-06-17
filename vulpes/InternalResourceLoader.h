#ifndef _VUL_INTERNALRESOURCELOADER_H
#define _VUL_INTERNALRESOURCELOADER_H
#include <cstdint>
#include <string>
#include "include/Mesh.h"
#include "include/Texture.h"
#include "include/Material.h"
#include "include/Handle.h"

// Intended to be a quick way to load resources internally, but it
// should be noted that there is no resource cache so duplicate
// data can be loaded. Primarily created to load deferred shaders

class InternalResourceLoader
{
public:
	static vul::Handle<vul::Mesh> loadMeshFromFile(const std::string& path);
	static vul::Handle<vul::Mesh> loadMeshFromData(float* vertices, uint32_t vertexCount,
		uint32_t* indices, uint32_t indexCount, float* normals = 0, float* uvcoords = 0,
		float* diffuse = 0);

	static vul::Handle<vul::Texture> loadTextureFromFile(const std::string& path);

	static vul::Handle<vul::Material> loadMaterialFromFile(const std::string& vsPath, const std::string& fsPath);
	static vul::Handle<vul::Material> loadMaterialFromText(const int8_t* vsContent, const int8_t* fsContent);

private:
	inline static bool validateShader(uint32_t shaderHandle);
	inline static bool validateProgram(uint32_t programHandle);
};

#endif // _VUL_INTERNALRESOURCELOADER_H