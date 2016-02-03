#define VULPESENGINE_EXPORT
#include "include/SceneObject.h"

namespace vul
{
	SceneObject::SceneObject(uint32_t id) : m_id(id)
	{
	}

	SceneObject::SceneObject(const SceneObject& other)
		: m_id(other.m_id), m_transformation(other.m_transformation)
	{
	}

	uint32_t SceneObject::getID() const
	{
		return m_id;
	}

	Transformation& SceneObject::getTransformation()
	{
		return m_transformation;
	}
}