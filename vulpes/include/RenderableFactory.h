#ifndef _VUL_RENDERABLEFACTORY_H
#define _VUL_RENDERABLEFACTORY_H
#include <string>
#include "Export.h"
#include "RenderableObject.h"

namespace vul
{
	class ResourceLoader;

	class VEAPI RenderableFactory
	{
	public:
		RenderableFactory(ResourceLoader*);

		uint32_t loadRenderable(const std::string& meshPath,
			const std::string& texturePath, Material* material);

		uint32_t createPlane(const std::string& texturePath, Material* material);
		uint32_t createCylinder(int32_t resolution, float height, Material* material);
		uint32_t createSphere();
		uint32_t createCube();

	private:
		ResourceLoader* m_resourceLoader;
		uint32_t m_currentID;
	};
}

#endif // _VUL_RENDERABLEFACTORY_H