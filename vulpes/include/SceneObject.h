#ifndef _VUL_SCENEOBJECT_H
#define _VUL_SCENEOBJECT_H
#include "Export.h"
#include "Transformation.h"

namespace vul
{
	class VEAPI SceneObject
	{
	public:
		SceneObject(uint32_t id);
		SceneObject(const SceneObject&);

		inline uint32_t getID() const;
		Transformation& getTransformation();

	protected:
		uint32_t m_id;
		Transformation m_transformation;
	};
}

#endif // _VUL_SCENEOBJECT_H