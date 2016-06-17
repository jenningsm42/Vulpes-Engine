#ifndef _VUL_SCENEOBJECT_H
#define _VUL_SCENEOBJECT_H
#include "Export.h"
#include "Transformation.h"

namespace vul
{
	enum struct SceneObjectType
	{
		Renderable,
		PointLight,
		Other
	};

	class VEAPI SceneObject
	{
	public:
		SceneObject(uint32_t id, SceneObjectType type);
		SceneObject(const SceneObject&);

		inline uint32_t getID() const;
		inline SceneObjectType getType() const;
		Transformation& getTransformation();

	protected:
		uint32_t m_id;
		SceneObjectType m_type;
		Transformation m_transformation;
	};
}

#endif // _VUL_SCENEOBJECT_H