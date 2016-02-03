#ifndef _VUL_RENDERABLEOBJECTPARAMETERS_H
#define _VUL_RENDERABLEOBJECTPARAMETERS_H
#include <cstdint>
#include "MeshResource.h"
#include "TextureResource.h"

namespace vul
{
	class Material;

	struct RenderableObjectParameters
	{
		uint32_t id;
		MeshResource meshResource;
		TextureResource textureResource;
		Material* material;
	};
}

#endif // _VUL_RENDERABLEOBJECTPARAMETERS_H