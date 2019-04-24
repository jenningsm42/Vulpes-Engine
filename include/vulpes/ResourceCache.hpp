#ifndef _VUL_RESOURCECACHE_HPP
#define _VUL_RESOURCECACHE_HPP

#include <map>
#include <string>

#include "Export.hpp"
#include "Handle.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Skeleton.hpp"
#include "Texture.hpp"

namespace vul {
    class VEAPI ResourceCache {
    public:
        ResourceCache();
        ~ResourceCache();

        void addMesh(const std::string& path, Handle<Mesh> resource);
        void addTexture(const std::string& path, Handle<Texture> resource);
        void addShader(const std::string& path, Handle<Shader> resource);
        void addSkeleton(const std::string& path, Handle<Skeleton> resource);

        bool hasResource(const std::string& path);
        Handle<Mesh> getMesh(const std::string& path);
        Handle<Texture> getTexture(const std::string& path);
        Handle<Shader> getShader(const std::string& path);
        Handle<Skeleton> getSkeleton(const std::string& path);

    private:
        std::map<std::string, Handle<Mesh>> m_meshes;
        std::map<std::string, Handle<Texture>> m_textures;
        std::map<std::string, Handle<Shader>> m_shaders;
        std::map<std::string, Handle<Skeleton>> m_skeletons;
    };
}

#endif // _VUL_RESOURCECACHE_HPP
