#ifndef _VUL_RESOURCELOADER_H
#define _VUL_RESOURCELOADER_H
#include <string>
#include <vector>
#include <cstdint>
#include "Export.h"
#include "ResourceCache.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "VEMParser.h"
#include "ImageParser.h"
#include "MeshData.h"

namespace vul
{
	class VEAPI ResourceLoader
	{
	public:
		ResourceLoader();
		~ResourceLoader();

		Handle<Mesh> loadMeshFromFile(const std::string& path);
		Handle<Mesh> loadMeshFromData(const MeshData&);

		Handle<Texture> loadTextureFromFile(const std::string& path);
		Handle<Texture> loadTextureFromColor(float red, float green, float blue);

		Handle<Texture> loadCubeMap(const std::string& frontPath,
									const std::string& backPath,
									const std::string& topPath,
									const std::string& bottomPath,
									const std::string& leftPath,
									const std::string& rightPath,
									bool prefilter = false);
		Handle<Texture> loadCubeMapCross(const std::string& path, bool prefilter = false);

		Handle<Shader> loadShaderFromFile(const std::string& vsPath, const std::string& fsPath);
		Handle<Shader> loadShaderFromText(const uint8_t* vsContent, const uint8_t* fsContent);

		Handle<Mesh> getPlane();
		Handle<Mesh> getSphere();
		Handle<Mesh> getQuad();

		Handle<ResourceCache> getResourceCache();

	private:
		ResourceCache m_resourceCache;
		VEMParser m_parserVEM;
		ImageParser m_imageParser;

		bool validateShader(uint32_t shaderHandle);
		bool validateProgram(uint32_t programHandle);

		void createPlane();
		void createSphere();
		void createQuad();
		void createIBLLUT(); // image-based light look up texture

		std::vector<uint8_t> readFile(const std::string& path); // Return empty string if file not found

		bool loadCubeMapSide(const std::string& path, Handle<Texture> texture,
			uint32_t side, uint32_t* width = nullptr);
		void prefilterCubeMap(Handle<Texture>&, uint32_t width);
	};
}

#endif // _VUL_RESOURCELOADER_H