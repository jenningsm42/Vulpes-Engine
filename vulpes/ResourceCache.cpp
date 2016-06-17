#define VULPESENGINE_EXPORT
#include "include/ResourceCache.h"

namespace vul
{
	ResourceCache::ResourceCache()
	{
	}

	ResourceCache::~ResourceCache()
	{
	}

	bool ResourceCache::hasResource(const std::string& path)
	{
		bool hasMesh = (m_meshes.find(path) != m_meshes.end());
		bool hasTexture = (m_textures.find(path) != m_textures.end());
		return hasMesh || hasTexture;
	}

	Handle<Mesh> ResourceCache::getMesh(const std::string& path)
	{
		return m_meshes[path];
	}

	Handle<Texture> ResourceCache::getTexture(const std::string& path)
	{
		return m_textures[path];
	}

	void ResourceCache::addMesh(const std::string& path, Handle<Mesh> mesh)
	{
		m_meshes[path] = mesh;
	}

	void ResourceCache::addTexture(const std::string& path, Handle<Texture> texture)
	{
		m_textures[path] = texture;
	}
}