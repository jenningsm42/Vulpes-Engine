#ifndef _VUL_TRANSFORMATION_H
#define _VUL_TRANSFORMATION_H
#include "glm/glm.hpp"
#include "Export.h"
#include <cstdint>

namespace vul
{
	class VEAPI Transformation
	{
	public:
		Transformation();
		Transformation(const Transformation&);

		inline void setPosition(float x, float y, float z);
		inline void setPosition(const glm::vec3&);
		inline void setX(float x);
		inline void setY(float y);
		inline void setZ(float z);

		inline const glm::vec3& getPosition() const;

		inline void setRotation(float xRot, float yRot, float zRot);
		inline void setRotation(const glm::vec3&);
		inline void setXRotation(float xRot);
		inline void setYRotation(float yRot);
		inline void setZRotation(float zRot);

		inline const glm::vec3& getRotation() const;

		inline void setScale(float scale);
		inline void setScale(float xScale, float yScale, float zScale);
		inline void setScale(const glm::vec3&);

		inline const glm::vec3& getScale() const;

		const glm::mat4& getTransformationMatrix() const;
		const glm::mat3& getNormalMatrix() const;

	private:
		glm::vec3 m_position;
		glm::vec3 m_rotation; // Angles in radians
		glm::vec3 m_scale;

		glm::mat4 m_positionMatrix;
		glm::mat4 m_rotationMatrix;
		glm::mat4 m_scaleMatrix;
		glm::mat4 m_transformationMatrix;
		glm::mat3 m_normalMatrix;

		void calculatePositionMatrix();
		void calculateRotationMatrix();
		void calculateScaleMatrix();
	};
}

#endif // _VUL_TRANSFORMATION_H