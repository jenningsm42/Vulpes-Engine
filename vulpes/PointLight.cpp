#define VULPESENGINE_EXPORT
#include "include/PointLight.h"

namespace vul
{
	PointLight::PointLight(uint32_t id) : m_intensity(10.f),
		SceneObject(id, SceneObjectType::PointLight)
	{
	}

	PointLight::PointLight(const PointLight& other) : SceneObject(other)
	{
		m_color = other.m_color;
		m_intensity = other.m_intensity;
	}

	void PointLight::setColor(const glm::vec3& color)
	{
		m_color = color;
	}

	void PointLight::setIntensity(float intensity)
	{
		m_intensity = intensity;
	}

	const glm::vec3& PointLight::getColor()
	{
		return m_color;
	}

	float PointLight::getIntensity()
	{
		return m_intensity;
	}
}