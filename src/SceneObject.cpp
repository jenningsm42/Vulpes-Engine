#define VULPESENGINE_EXPORT
#include "include/SceneObject.h"

namespace vul
{
	SceneObject::SceneObject(uint32_t id, SceneObjectType type) : m_id(id), m_type(type)
	{
	}

	SceneObject::SceneObject(const SceneObject& other)
		: m_id(other.m_id), m_transformation(other.m_transformation), m_type(other.m_type)
	{
	}

	uint32_t SceneObject::getID() const
	{
		return m_id;
	}

	SceneObjectType SceneObject::getType() const
	{
		return m_type;
	}

	Transformation& SceneObject::getTransformation()
	{
		return m_transformation;
	}
}