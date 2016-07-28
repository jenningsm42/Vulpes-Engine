#define VULPESENGINE_EXPORT
#include "include/RenderableObject.h"

namespace vul
{
	RenderableObject::RenderableObject(uint32_t id)
		: SceneObject(id, SceneObjectType::Renderable),
		m_reflectionCoefficient(0.15f), // From polycarbonate refractive index
		m_visible(true)
	{
	}

	RenderableObject::RenderableObject(const RenderableObject& other) : SceneObject(other)
	{
		m_mesh = other.m_mesh;
		m_colorMap = other.m_colorMap;
		m_normalMap = other.m_normalMap;
		m_roughnessMap = other.m_roughnessMap;
		m_reflectionCoefficient = other.m_reflectionCoefficient;
		m_visible = other.m_visible;
	}

	void RenderableObject::attachMesh(const Handle<Mesh>& mesh)
	{
		m_mesh = mesh.makeCopy();
	}

	void RenderableObject::attachColorMap(const Handle<Texture>& tex)
	{
		m_colorMap = tex.makeCopy();
	}

	void RenderableObject::attachRoughnessMap(const Handle<Texture>& tex)
	{
		m_roughnessMap = tex.makeCopy();
	}

	void RenderableObject::attachNormalMap(const Handle<Texture>& tex)
	{
		m_normalMap = tex.makeCopy();
	}

	void RenderableObject::setRefractiveIndex(float n)
	{
		float tmp = (n - 1) / (n + 1);
		m_reflectionCoefficient = tmp * tmp;
	}

	void RenderableObject::setVisible(bool visible)
	{
		m_visible = visible;
	}

	Handle<Mesh> RenderableObject::getMesh()
	{
		return Handle<Mesh>(m_mesh);
	}

	Handle<Texture> RenderableObject::getColorMap()
	{
		return Handle<Texture>(m_colorMap);
	}

	Handle<Texture> RenderableObject::getNormalMap()
	{
		return Handle<Texture>(m_normalMap);
	}

	Handle<Texture> RenderableObject::getRoughnessMap()
	{
		return Handle<Texture>(m_roughnessMap);
	}

	float RenderableObject::getReflectionCoefficient()
	{
		return m_reflectionCoefficient;
	}

	bool RenderableObject::getVisible()
	{
		return m_visible;
	}
}