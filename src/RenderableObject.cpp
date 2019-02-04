#define VULPESENGINE_EXPORT

#include <vulpes/RenderableObject.hpp>

namespace vul
{
	RenderableObject::RenderableObject(uint32_t id)
		: SceneObject(id, SceneObjectType::Renderable), m_flags(0)
	{
		m_flags |= static_cast<uint8_t>(RenderableObjectFlags::Visible);
	}

	RenderableObject::RenderableObject(const RenderableObject& other) : SceneObject(other)
	{
		m_mesh = other.m_mesh;
		m_colorMap = other.m_colorMap;
		m_normalMap = other.m_normalMap;
		m_roughnessMap = other.m_roughnessMap;
		m_metalMap = other.m_metalMap;
		m_flags = other.m_flags;
	}

	void RenderableObject::attachMesh(const Handle<Mesh>& mesh)
	{
		m_flags |= static_cast<uint8_t>(RenderableObjectFlags::MeshAttached);
		m_mesh = mesh.makeCopy();
	}

	void RenderableObject::attachColorMap(const Handle<Texture>& tex)
	{
		m_flags |= static_cast<uint8_t>(RenderableObjectFlags::ColorMapAttached);
		m_colorMap = tex.makeCopy();
	}

	void RenderableObject::attachNormalMap(const Handle<Texture>& tex)
	{
		m_flags |= static_cast<uint8_t>(RenderableObjectFlags::NormalMapAttached);
		m_normalMap = tex.makeCopy();
	}

	void RenderableObject::attachRoughnessMap(const Handle<Texture>& tex)
	{
		m_flags |= static_cast<uint8_t>(RenderableObjectFlags::RoughnessMapAttached);
		m_roughnessMap = tex.makeCopy();
	}

	void RenderableObject::attachMetalMap(const Handle<Texture>& tex)
	{
		m_flags |= static_cast<uint8_t>(RenderableObjectFlags::MetalMapAttached);
		m_metalMap = tex.makeCopy();
	}

	void RenderableObject::setVisible(bool visible)
	{
		visible ? m_flags |= static_cast<uint8_t>(RenderableObjectFlags::Visible)
			    : m_flags &= ~static_cast<uint8_t>(RenderableObjectFlags::Visible);
	}

	Handle<Mesh> RenderableObject::getMesh()
	{
		return m_flags & static_cast<uint8_t>(RenderableObjectFlags::MeshAttached)?
			Handle<Mesh>(m_mesh) : Handle<Mesh>();
	}

	Handle<Texture> RenderableObject::getColorMap()
	{
		return m_flags & static_cast<uint8_t>(RenderableObjectFlags::ColorMapAttached)?
			Handle<Texture>(m_colorMap) : Handle<Texture>();
	}

	Handle<Texture> RenderableObject::getNormalMap()
	{
		return m_flags & static_cast<uint8_t>(RenderableObjectFlags::NormalMapAttached)?
			Handle<Texture>(m_normalMap) : Handle<Texture>();
	}

	Handle<Texture> RenderableObject::getRoughnessMap()
	{
		return m_flags & static_cast<uint8_t>(RenderableObjectFlags::RoughnessMapAttached)?
			Handle<Texture>(m_roughnessMap) : Handle<Texture>();
	}

	Handle<Texture> RenderableObject::getMetalMap()
	{
		return m_metalMap;
	}

	bool RenderableObject::isVisible()
	{
		return m_flags & static_cast<uint8_t>(RenderableObjectFlags::Visible);
	}
}
