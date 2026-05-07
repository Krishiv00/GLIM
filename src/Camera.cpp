#include <cmath>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLIM/Graphics/Camera.hpp"

using namespace gl;

void Camera3D::clampRotation() {
    m_Rotation.x = std::fmodf(m_Rotation.x, glm::radians<float>(360.f));
    m_Rotation.y = glm::clamp(m_Rotation.y, -MaxPitch, MaxPitch);
}

void Camera3D::updateForward() {
    const float cos_y = glm::cos(m_Rotation.x);
    const float sin_y = glm::sin(m_Rotation.x);

    const float cos_p = glm::cos(m_Rotation.y);
    const float sin_p = glm::sin(m_Rotation.y);

    m_Forward = glm::normalize(glm::vec3(
        cos_y * cos_p, sin_p, sin_y * cos_p
    ));
}

void Camera3D::updateVpMatrix() {
    const glm::mat4 projectionMat = glm::perspective(m_FOV, m_Aspect, m_Near, m_Far);
    const glm::mat4 viewMat = glm::lookAt(m_Position, m_Position + m_Forward, Up);

    m_VPMat = projectionMat * viewMat;
}

Camera3D::Camera3D() {
    clampRotation();
    updateForward();
    updateVpMatrix();
}

void Camera3D::Move(glm::vec3 delta) {
    m_Position += delta;

    updateVpMatrix();
}

void Camera3D::Rotate(glm::vec2 delta) {
    m_Rotation += delta;

    clampRotation();
    updateForward();
    updateVpMatrix();
}

void Camera3D::SetPosition(glm::vec3 position) {
    m_Position = position;

    updateVpMatrix();
}

void Camera3D::SetRotation(glm::vec2 rotation) {
    m_Rotation = rotation;

    clampRotation();
    updateForward();
    updateVpMatrix();
}

void Camera3D::SetFov(unsigned int fov) {
    m_FOV = glm::radians<float>(fov);

    updateVpMatrix();
}

void Camera3D::SetAspect(float aspect) {
    m_Aspect = aspect;

    updateVpMatrix();
}

void Camera3D::SetClipping(float near, float far) {
    m_Near = near;
    m_Far = far;

    updateVpMatrix();
}

glm::vec3 Camera3D::GetRight() const {
    return glm::normalize(glm::cross(m_Forward, Up));
}

glm::vec3 gl::Camera3D::GetFront() const {
    return glm::normalize(glm::vec3(m_Forward.x, 0.f, m_Forward.z));
}