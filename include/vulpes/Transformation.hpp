#ifndef _VUL_TRANSFORMATION_HPP
#define _VUL_TRANSFORMATION_HPP

#include <cstdint>

#include <glm/glm.hpp>

#include "Export.hpp"

namespace vul
{
	class VEAPI Transformation
	{
	public:
		Transformation();
		Transformation(const Transformation&);

		void setPosition(float x, float y, float z);
		void setPosition(const glm::vec3&);
		void setX(float x);
		void setY(float y);
		void setZ(float z);

		const glm::vec3& getPosition() const;

		void setRotation(float xRot, float yRot, float zRot);
		void setRotation(const glm::vec3&);
		void setXRotation(float xRot);
		void setYRotation(float yRot);
		void setZRotation(float zRot);

		const glm::vec3& getRotation() const;

		void setScale(float scale);
		void setScale(float xScale, float yScale, float zScale);
		void setScale(const glm::vec3&);

		const glm::vec3& getScale() const;

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

#endif // _VUL_TRANSFORMATION_HPP
