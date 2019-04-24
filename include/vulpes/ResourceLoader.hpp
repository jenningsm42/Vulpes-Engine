#ifndef _VUL_RESOURCELOADER_HPP
#define _VUL_RESOURCELOADER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "Export.hpp"
#include "ImageParser.hpp"
#include "Mesh.hpp"
#include "MeshData.hpp"
#include "ResourceCache.hpp"
#include "Shader.hpp"
#include "Skeleton.hpp"
#include "Texture.hpp"
#include "VEMParser.hpp"
#include "VESParser.hpp"

namespace vul {
    class VEAPI ResourceLoader {
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

        Handle<Skeleton> loadSkeletonFromFile(const std::string& path);

        Handle<Mesh> getPlane();
        Handle<Mesh> getSphere();
        Handle<Mesh> getQuad();

        Handle<Mesh> generateSkeletonMesh(Handle<Skeleton>);

        Handle<ResourceCache> getResourceCache();

    private:
        ResourceCache m_resourceCache;
        VEMParser m_parserVEM;
        VESParser m_parserVES;
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

#endif // _VUL_RESOURCELOADER_HPP
