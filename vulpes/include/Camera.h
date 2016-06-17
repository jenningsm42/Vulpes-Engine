#ifndef _VUL_CAMERA_H
#define _VUL_CAMERA_H
#include "glm/glm.hpp"
#include "Export.h"
#include <cstdint>

namespace vul
{
	class Engine;

	class VEAPI Camera
	{
	public:
		Camera(Engine& engine, const glm::vec3& position = glm::vec3(0, 0, 1),
			const glm::vec3& up = glm::vec3(0, 1, 0),
			const glm::vec3& target = glm::vec3(0, 0, 0),
			float near = .1f, float far = 500.f, float fieldOfView = 1.047196667f);
		~Camera();

		void setPosition(const glm::vec3&);
		void setUp(const glm::vec3&);
		void setTarget(const glm::vec3&);

		void setNear(float);
		void setFar(float);
		void setFieldOfView(float);

		void update();

		inline glm::mat4 getViewMatrix();
		inline glm::mat4 getProjMatrix();
		inline glm::mat4 getTranslationMatrix();

		inline float getNear();
		inline float getFar();

		inline glm::vec3 getPosition();
		inline glm::vec3 getTarget();
		inline float getFieldOfView();
		inline float getAspectRatio();

		inline uint32_t getWidth();
		inline uint32_t getHeight();

	protected:
		uint32_t m_width, m_height;
		glm::vec3 m_position;
		glm::vec3 m_up;
		glm::vec3 m_target;
		float m_near, m_far;
		float m_fieldOfView; // In radians

		glm::mat4 m_projMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_translateMatrix;
	};
}

#endif // _VUL_CAMERA_H