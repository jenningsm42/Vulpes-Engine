#define VULPESENGINE_EXPORT
#include "include/PointLight.h"

namespace vul
{
	PointLight::PointLight(uint32_t id) : m_brightness(10.f), m_radius(10.f),
		m_color(glm::vec3(1.f, 1.f, 1.f)), SceneObject(id, SceneObjectType::PointLight)
	{
	}

	PointLight::PointLight(const PointLight& other) : SceneObject(other)
	{
		m_color = other.m_color;
		m_brightness = other.m_brightness;
		m_radius = other.m_radius;
	}

	void PointLight::setColor(const glm::vec3& color)
	{
		m_color = color;
	}

	void PointLight::setBrightness(float intensity)
	{
		m_brightness = intensity;
	}

	void PointLight::setRadius(float radius)
	{
		m_radius = radius;
	}

	const glm::vec3& PointLight::getColor()
	{
		return m_color;
	}

	float PointLight::getBrightness()
	{
		return m_brightness;
	}

	float PointLight::getRadius()
	{
		return m_radius;
	}
}