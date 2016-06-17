#define VULPESENGINE_EXPORT
#include "include/RenderableObject.h"

namespace vul
{
	RenderableObject::RenderableObject(uint32_t id)
		: SceneObject(id, SceneObjectType::Renderable),
		m_reflectionCoefficient(0.15f), // From polycarbonate refractive index
		m_roughness(0.5f)
	{
	}

	RenderableObject::RenderableObject(const RenderableObject& other) : SceneObject(other)
	{
		m_mesh = other.m_mesh;
		m_colorMap = other.m_colorMap;
		m_specularMap = other.m_specularMap;
		m_normalMap = other.m_normalMap;
		m_reflectionCoefficient = other.m_reflectionCoefficient;
		m_roughness = other.m_roughness;
	}

	void RenderableObject::attachMesh(const Handle<Mesh>& mesh)
	{
		m_mesh = mesh.makeCopy();
	}

	void RenderableObject::attachColorMap(const Handle<Texture>& tex)
	{
		m_colorMap = tex.makeCopy();
	}

	void RenderableObject::attachSpecularMap(const Handle<Texture>& tex)
	{
		m_specularMap = tex.makeCopy();
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

	void RenderableObject::setRoughness(float roughness)
	{
		m_roughness = roughness;
	}

	Handle<Mesh> RenderableObject::getMesh()
	{
		return Handle<Mesh>(m_mesh);
	}

	Handle<Texture> RenderableObject::getColorMap()
	{
		return Handle<Texture>(m_colorMap);
	}

	float RenderableObject::getReflectionCoefficient()
	{
		return m_reflectionCoefficient;
	}

	float RenderableObject::getRoughness()
	{
		return m_roughness;
	}
}