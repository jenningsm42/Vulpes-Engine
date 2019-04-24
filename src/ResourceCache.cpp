#define VULPESENGINE_EXPORT

#include <vulpes/ResourceCache.hpp>

namespace vul {
    ResourceCache::ResourceCache() {
    }

    ResourceCache::~ResourceCache() {
    }

    bool ResourceCache::hasResource(const std::string& path) {
        bool hasMesh = (m_meshes.find(path) != m_meshes.end());
        bool hasTexture = (m_textures.find(path) != m_textures.end());
        bool hasShader = (m_shaders.find(path) != m_shaders.end());
        bool hasSkeleton = (m_skeletons.find(path) != m_skeletons.end());
        return hasMesh || hasTexture || hasShader || hasSkeleton;
    }

    Handle<Mesh> ResourceCache::getMesh(const std::string& path) {
        return m_meshes[path];
    }

    Handle<Texture> ResourceCache::getTexture(const std::string& path) {
        return m_textures[path];
    }

    Handle<Shader> ResourceCache::getShader(const std::string& path) {
        return m_shaders[path];
    }

    Handle<Skeleton> ResourceCache::getSkeleton(const std::string& path) {
        return m_skeletons[path];
    }

    void ResourceCache::addMesh(const std::string& path, Handle<Mesh> mesh) {
        m_meshes[path] = mesh;
    }

    void ResourceCache::addTexture(const std::string& path, Handle<Texture> texture) {
        m_textures[path] = texture;
    }

    void ResourceCache::addShader(const std::string& path, Handle<Shader> shader) {
        m_shaders[path] = shader;
    }

    void ResourceCache::addSkeleton(const std::string& path, Handle<Skeleton> skeleton) {
        m_skeletons[path] = skeleton;
    }
}
