#define VULPESENGINE_EXPORT

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulpes/Camera.hpp>
#include <vulpes/Engine.hpp>
#include <vulpes/Window.hpp>

namespace vul {
    Camera::Camera(Engine& engine, const glm::vec3& position, const glm::vec3& target,
        const glm::vec3& up, float near, float far, float fieldOfView)
        : m_position(position), m_up(up), m_target(target), m_near(near),
        m_far(far), m_fieldOfView(fieldOfView) {
        m_width = engine.getWindow()->getWidth();
        m_height = engine.getWindow()->getHeight();
        update();
    }

    Camera::~Camera() {
    }

    void Camera::setPosition(const glm::vec3& position) {
        m_position = position;
    }

    void Camera::setUp(const glm::vec3& up) {
        m_up = up;
    }

    void Camera::setTarget(const glm::vec3& target) {
        m_target = target;
    }

    void Camera::setNear(float near) {
        m_near = near;
    }

    void Camera::setFar(float far) {
        m_far = far;
    }

    void Camera::setFieldOfView(float fieldOfView) {
        m_fieldOfView = fieldOfView;
    }

    void Camera::update() {
        m_projMatrix = glm::perspective(m_fieldOfView,
            (float)m_width / (float)m_height, m_near, m_far);
        m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
        m_translateMatrix = glm::translate(glm::mat4(1.f), m_position);
    }

    glm::mat4 Camera::getViewMatrix() {
        return m_viewMatrix;
    }

    glm::mat4 Camera::getProjMatrix() {
        return m_projMatrix;
    }

    glm::mat4 Camera::getTranslationMatrix() {
        return m_translateMatrix;
    }

    float Camera::getNear() {
        return m_near;
    }

    float Camera::getFar() {
        return m_far;
    }

    glm::vec3 Camera::getPosition() {
        return m_position;
    }

    glm::vec3 Camera::getTarget() {
        return m_target;
    }

    float Camera::getFieldOfView() {
        return m_fieldOfView;
    }

    float Camera::getAspectRatio() {
        return (float)m_width / (float)m_height;
    }

    uint32_t Camera::getWidth() {
        return m_width;
    }

    uint32_t Camera::getHeight() {
        return m_height;
    }
}
