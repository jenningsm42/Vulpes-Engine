#define VULPESENGINE_EXPORT
#define GLM_ENABLE_EXPERIMENTAL

#include <cstdio>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vulpes/Transformation.hpp>

namespace vul
{
	Transformation::Transformation() : m_position(glm::vec3()), m_rotation(glm::vec3()),
		m_scale(glm::vec3(1.f, 1.f, 1.f))
	{
		m_positionMatrix = glm::translate(glm::mat4(1.f), m_position);
		m_rotationMatrix = glm::eulerAngleYXZ(m_rotation.y, m_rotation.x, m_rotation.z);
		m_normalMatrix = glm::mat3(m_rotationMatrix);
		m_scaleMatrix = glm::scale(glm::mat4(1.f), m_scale);
		m_transformationMatrix = m_positionMatrix * m_rotationMatrix * m_scaleMatrix;
	}

	Transformation::Transformation(const Transformation& other)
	{
		m_position = other.m_position;
		m_rotation = other.m_rotation;
		m_scale = other.m_scale;
		m_positionMatrix = other.m_positionMatrix;
		m_rotationMatrix = other.m_rotationMatrix;
		m_normalMatrix = other.m_normalMatrix;
		m_scaleMatrix = other.m_scaleMatrix;
		m_transformationMatrix = other.m_transformationMatrix;
	}

	// Position

	void Transformation::setPosition(float x, float y, float z)
	{
		setPosition(glm::vec3(x, y, z));
	}

	void Transformation::setPosition(const glm::vec3& position)
	{
		m_position = position;
		calculatePositionMatrix();
	}

	void Transformation::setX(float x)
	{
		m_position.x = x;
		calculatePositionMatrix();
	}

	void Transformation::setY(float y)
	{
		m_position.y = y;
		calculatePositionMatrix();
	}

	void Transformation::setZ(float z)
	{
		m_position.z = z;
		calculatePositionMatrix();
	}

	const glm::vec3& Transformation::getPosition() const
	{
		return m_position;
	}

	// Rotation

	void Transformation::setRotation(float x, float y, float z)
	{
		setRotation(glm::vec3(x, y, z));
	}

	void Transformation::setRotation(const glm::vec3& rotation)
	{
		m_rotation = rotation;
		calculateRotationMatrix();
	}

	void Transformation::setXRotation(float x)
	{
		m_rotation.x = x;
		calculateRotationMatrix();
	}

	void Transformation::setYRotation(float y)
	{
		m_rotation.y = y;
		calculateRotationMatrix();
	}

	void Transformation::setZRotation(float z)
	{
		m_rotation.z = z;
		calculateRotationMatrix();
	}

	const glm::vec3& Transformation::getRotation() const
	{
		return m_rotation;
	}

	// Scale

	void Transformation::setScale(float scale)
	{
		setScale(glm::vec3(scale, scale, scale));
	}

	void Transformation::setScale(float x, float y, float z)
	{
		setScale(glm::vec3(x, y, z));
	}

	void Transformation::setScale(const glm::vec3& scale)
	{
		m_scale = scale;
		calculateScaleMatrix();
	}

	const glm::vec3& Transformation::getScale() const
	{
		return m_scale;
	}

	const glm::mat4& Transformation::getTransformationMatrix() const
	{
		return m_transformationMatrix;
	}

	const glm::mat3& Transformation::getNormalMatrix() const
	{
		return m_normalMatrix;
	}

	void Transformation::calculatePositionMatrix()
	{
		m_positionMatrix = glm::translate(glm::mat4(1.f), m_position);
		m_transformationMatrix = m_positionMatrix * m_rotationMatrix * m_scaleMatrix;
	}

	void Transformation::calculateRotationMatrix()
	{
		m_rotationMatrix = glm::eulerAngleYXZ(m_rotation.y, m_rotation.x, m_rotation.z);
		m_transformationMatrix = m_positionMatrix * m_rotationMatrix * m_scaleMatrix;
		m_normalMatrix = glm::mat3(m_rotationMatrix);
	}

	void Transformation::calculateScaleMatrix()
	{
		m_scaleMatrix = glm::scale(glm::mat4(1.f), m_scale);
		m_transformationMatrix = m_positionMatrix * m_rotationMatrix * m_scaleMatrix;
	}
}
