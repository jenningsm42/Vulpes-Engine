#ifndef _VUL_RESOURCECACHE_H
#define _VUL_RESOURCECACHE_H
#include <map>
#include <string>
#include "Export.h"
#include "Handle.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

namespace vul
{
	class VEAPI ResourceCache
	{
	public:
		ResourceCache();
		~ResourceCache();

		bool hasResource(const std::string& path);
		Handle<Mesh> getMesh(const std::string& path);
		Handle<Texture> getTexture(const std::string& path);

	private:
		std::map<std::string, Handle<Mesh>> m_meshes;
		std::map<std::string, Handle<Texture>> m_textures;

		void addMesh(const std::string& path, Handle<Mesh> resource);
		void addTexture(const std::string& path, Handle<Texture> resource);

		friend class ResourceLoader;
	};
}

#endif // _VUL_RESOURCECACHE_H