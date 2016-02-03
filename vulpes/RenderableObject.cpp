#define VULPESENGINE_EXPORT
#include "include/RenderableObject.h"

namespace vul
{
	RenderableObject::RenderableObject(const RenderableObjectParameters& param)
		: SceneObject(param.id)
	{
		m_meshResource = param.meshResource;
		m_textureResource = param.textureResource;
		m_material = param.material;
	}

	RenderableObject::RenderableObject(const RenderableObject& other) : SceneObject(other)
	{
		m_meshResource = other.m_meshResource;
		m_textureResource = other.m_textureResource;
		m_material = other.m_material;
		for(int32_t i = 0; i < 8; i++)
			m_materialUniforms[i] = other.m_materialUniforms[i];
	}

	const MeshResource& RenderableObject::getMeshResource() const
	{
		return m_meshResource;
	}

	const TextureResource& RenderableObject::getTextureResource() const
	{
		return m_textureResource;
	}

	Material* RenderableObject::getMaterial() 
	{
		return m_material;
	}
}