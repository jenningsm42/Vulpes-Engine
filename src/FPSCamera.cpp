#define VULPESENGINE_EXPORT

#include <vulpes/Engine.hpp>
#include <vulpes/FPSCamera.hpp>
#include <vulpes/Window.hpp>

namespace vul
{
	FPSCamera::FPSCamera(Engine& engine, bool fly) : Camera(engine),
		m_speed(5.f), m_hAngle(-3.14159f / 2.f), m_vAngle(-3.14159f / 2.f),
		m_sensitivity(.003f), m_fly(fly), m_ih(engine.getInputHandler())
	{
		engine.getWindow()->hideCursor();
		m_halfWidth = engine.getWindow()->getWidth() / 2;
		m_halfHeight = engine.getWindow()->getHeight() / 2;

		m_ih->setCursorPos(m_halfWidth, m_halfHeight);
	}

	FPSCamera::~FPSCamera()
	{
	}

	void FPSCamera::setSpeed(float speed)
	{
		m_speed = speed;
	}

	void FPSCamera::setSensitivity(float sensitivity)
	{
		m_sensitivity = sensitivity;
	}

	void FPSCamera::update(float dt)
	{
		// Rotation (mouse)
		int dx = m_ih->getCursorX() - m_halfWidth;
		int dy = m_ih->getCursorY() - m_halfHeight;

		m_ih->setCursorPos(m_halfWidth, m_halfHeight);

		m_hAngle += (float)dx * m_sensitivity;
		m_vAngle -= (float)dy * m_sensitivity;

		const float pi = 3.14159f;
		if(m_vAngle > 0) m_vAngle = -.001f; // Camera flips at m_vAngle = 0
		else if(m_vAngle < -pi) m_vAngle = -pi;

		glm::vec3 direction = glm::vec3(cosf(m_hAngle), 0, sinf(m_hAngle));

		if(m_ih->getKeyDown(Key::W))
			m_position -= direction * dt * m_speed;
		if(m_ih->getKeyDown(Key::S))
			m_position += direction * dt * m_speed;
		if(m_ih->getKeyDown(Key::A))
			m_position += glm::vec3(-direction.z, 0, direction.x) * dt * m_speed;
		if(m_ih->getKeyDown(Key::D))
			m_position += glm::vec3(direction.z, 0, -direction.x) * dt * m_speed;
		if(m_fly && m_ih->getKeyDown(Key::Space))
			m_position += glm::vec3(0, m_speed * dt, 0);
		if(m_fly && m_ih->getKeyDown(Key::LeftControl))
			m_position -= glm::vec3(0, m_speed * dt, 0);

		m_target = glm::vec3(direction.x * sinf(m_vAngle), cosf(m_vAngle), direction.z * sinf(m_vAngle)) + m_position;

		// Update matrices
		Camera::update();
	}
}
