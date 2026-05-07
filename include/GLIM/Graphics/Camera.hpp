#pragma once

#include "glm/glm.hpp"

namespace gl {
    class Camera3D {
    public:
        static constexpr inline const glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f);
        static constexpr inline const float MaxPitch = glm::radians<float>(89.f);

    private:
        void clampRotation();
        void updateForward();
        void updateVpMatrix();

        glm::vec2 m_Rotation{0.f, 0.f};
        glm::vec3 m_Position{0.f, 0.f, 0.f};

        glm::vec3 m_Forward;

        float m_FOV{glm::radians<float>(60.f)};
        float m_Aspect{1.f};
        float m_Near{0.1f};
        float m_Far{1000.f};

        glm::mat4 m_VPMat;

    public:
        Camera3D();

        void Move(glm::vec3 delta);

        void Rotate(glm::vec2 delta);

        void SetPosition(glm::vec3 position);

        void SetRotation(glm::vec2 rotation);

        void SetFov(unsigned int fov);

        void SetAspect(float aspect);

        void SetClipping(float near, float far);

        [[nodiscard]]
        glm::vec3 GetRight() const;

        [[nodiscard]]
        glm::vec3 GetFront() const;

        [[nodiscard]]
        inline glm::vec2 GetRotation() const noexcept {
            return m_Rotation;
        }

        [[nodiscard]]
        inline glm::vec3 GetPosition() const noexcept {
            return m_Position;
        }

        [[nodiscard]]
        inline glm::vec3 GetForward() const noexcept {
            return m_Forward;
        }

        [[nodiscard]]
        inline glm::mat4 GetVP() const noexcept {
            return m_VPMat;
        }
    };
}