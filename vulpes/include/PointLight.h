#ifndef _VUL_POINTLIGHT_H
#define _VUL_POINTLIGHT_H
#include "Export.h"
#include "SceneObject.h"
#include "glm/glm.hpp"

namespace vul
{
	class VEAPI PointLight : public SceneObject
	{
	public:
		PointLight(uint32_t id);
		PointLight(const PointLight&);

		void setColor(const glm::vec3&);
		void setIntensity(float);

		const glm::vec3& getColor();
		float getIntensity();

	private:
		glm::vec3 m_color;
		float m_intensity;
	};
}

#endif // _VUL_POINTLIGHT_H