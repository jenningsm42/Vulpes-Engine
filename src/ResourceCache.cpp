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
        return hasMesh || hasTexture || hasShader;
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

    void ResourceCache::addMesh(const std::string& path, Handle<Mesh> mesh) {
        m_meshes[path] = mesh;
    }

    void ResourceCache::addTexture(const std::string& path, Handle<Texture> texture) {
        m_textures[path] = texture;
    }

    void ResourceCache::addShader(const std::string& path, Handle<Shader> shader) {
        m_shaders[path] = shader;
    }
}
