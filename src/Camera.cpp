#include <cmath>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLIM/Graphics/Camera.hpp"

namespace {
    static constexpr inline glm::vec2 ToGlm(gl::Vector2f v) noexcept {
        return glm::vec2(v.x, v.y);
    }

    static constexpr inline glm::vec3 ToGlm(gl::Vector3f v) noexcept {
        return glm::vec3(v.x, v.y, v.z);
    }
}

using namespace gl;

#pragma region Camera 2D

void Camera2D::updateVpMatrix() {
    const float w = m_Size.x / m_Zoom;
    const float h = m_Size.y / m_Zoom;

    const glm::mat4 projMat = glm::ortho(0.f, w, h, 0.f);

    const glm::mat4 viewMat = glm::rotate(
        glm::translate(glm::mat4(1.f), glm::vec3(ToGlm(-m_Position), 0.f)),
        -m_Rotation, glm::vec3(0.f, 0.f, 1.f)
    );

    m_VPMat = projMat * viewMat;
}

Camera2D::Camera2D() {
    updateVpMatrix();
}

gl::Camera2D::Camera2D(Vector2f position, Vector2f size) {
    m_Position = position;
    m_Size = size;

    updateVpMatrix();
}

void Camera2D::Move(Vector2f delta) {
    m_Position += delta;

    updateVpMatrix();
}

void Camera2D::Rotate(float delta) {
    m_Rotation = std::fmodf(m_Rotation + delta, glm::radians(360.f));

    updateVpMatrix();
}

void Camera2D::Zoom(float delta) {
    m_Zoom = glm::clamp(m_Zoom + delta, MinZoom, MaxZoom);

    updateVpMatrix();
}

void Camera2D::SetPosition(Vector2f position) {
    m_Position = position;

    updateVpMatrix();
}

void Camera2D::SetSize(Vector2f size) {
    m_Size = size;

    updateVpMatrix();
}

void Camera2D::SetRotation(float rotation) {
    m_Rotation = std::fmodf(rotation, glm::radians(360.f));

    updateVpMatrix();
}

void Camera2D::SetZoom(float zoom) {
    m_Zoom = glm::clamp(zoom, MinZoom, MaxZoom);

    updateVpMatrix();
}

#pragma region Camera 3D

void Camera3D::clampRotation() {
    m_Rotation.x = std::fmodf(m_Rotation.x, glm::radians<float>(360.f));
    m_Rotation.y = glm::clamp(m_Rotation.y, -MaxPitch, MaxPitch);
}

void Camera3D::updateForward() {
    const float cos_y = std::cos(m_Rotation.x);
    const float sin_y = std::sin(m_Rotation.x);

    const float cos_p = std::cos(m_Rotation.y);
    const float sin_p = std::sin(m_Rotation.y);

    m_Forward = Vector3f(
        cos_y * cos_p, sin_p, sin_y * cos_p
    ).Normalize();
}

void Camera3D::updateVpMatrix() {
    const glm::mat4 projectionMat = glm::perspective(m_FOV, m_Aspect, m_Near, m_Far);

    const glm::vec3 pos = ToGlm(m_Position);
    const glm::mat4 viewMat = glm::lookAt(pos, pos + ToGlm(m_Forward), ToGlm(Up));

    m_VPMat = projectionMat * viewMat;
}

Camera3D::Camera3D() {
    clampRotation();
    updateForward();
    updateVpMatrix();
}

gl::Camera3D::Camera3D(Vector3f position, Vector2f rotation) {
    m_Position = position;
    m_Rotation = rotation;

    clampRotation();
    updateForward();
    updateVpMatrix();
}

void Camera3D::Move(Vector3f delta) {
    m_Position += delta;

    updateVpMatrix();
}

void Camera3D::Rotate(Vector2f delta) {
    m_Rotation += delta;

    clampRotation();
    updateForward();
    updateVpMatrix();
}

void Camera3D::SetPosition(Vector3f position) {
    m_Position = position;

    updateVpMatrix();
}

void Camera3D::SetRotation(Vector2f rotation) {
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

Vector3f Camera3D::GetRight() const {
    return m_Forward.Cross(Up).Normalize();
}

Vector3f gl::Camera3D::GetFront() const {
    return Vector3f(m_Forward.x, 0.f, m_Forward.z).Normalize();
}