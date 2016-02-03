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
		return m_resources.find(path) != m_resources.end();
	}

	Resource* ResourceCache::getResource(const std::string& name)
	{
		return m_resources[name];
	}

	void ResourceCache::addResource(const std::string& name, Resource* resource)
	{
		m_resources[name] = resource;
	}
}