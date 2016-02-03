#ifndef _VUL_RESOURCECACHE_H
#define _VUL_RESOURCECACHE_H
#include <map>
#include <string>
#include "Export.h"
#include "MeshResource.h"
#include "TextureResource.h"
#include "MaterialResource.h"

namespace vul
{
	struct Resource;

	class VEAPI ResourceCache
	{
	public:
		ResourceCache();
		~ResourceCache();

		bool hasResource(const std::string& path);
		Resource* getResource(const std::string& path);

	private:
		std::map<std::string, Resource*> m_resources;

		void addResource(const std::string& path, Resource* resource);

		friend class ResourceLoader;
	};
}

#endif // _VUL_RESOURCECACHE_H