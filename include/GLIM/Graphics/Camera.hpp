#pragma once

#include "glm/glm.hpp"

namespace gl {
    class Camera {
    protected:
        glm::mat4 m_VPMat;

    public:
        [[nodiscard]]
        inline glm::mat4 GetVP() const noexcept {
            return m_VPMat;
        }
    };

    class Camera2D : public Camera {
    public:
        static constexpr inline const float MaxZoom = 100.f;
        static constexpr inline const float MinZoom = 0.01f;

    private:
        void updateVpMatrix();

        glm::vec2 m_Position{0.f, 0.f};
        glm::vec2 m_Size{1.f, 1.f};

        float m_Rotation{0.f};
        float m_Zoom{1.f};

    public:
        Camera2D();
        Camera2D(glm::vec2 position, glm::vec2 size);

        void Move(glm::vec2 delta);

        void Rotate(float delta);

        void Zoom(float delta);

        void SetPosition(glm::vec2 position);

        void SetSize(glm::vec2 size);

        void SetRotation(float rotation);

        void SetZoom(float zoom);

        [[nodiscard]]
        inline float GetRotation() const noexcept {
            return m_Rotation;
        }

        [[nodiscard]]
        inline glm::vec2 GetPosition() const noexcept {
            return m_Position;
        }

        [[nodiscard]]
        inline float GetZoom() const noexcept {
            return m_Zoom;
        }

        [[nodiscard]]
        inline glm::vec2 GetSize() const noexcept {
            return m_Size;
        }
    };

    class Camera3D : public Camera {
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

    public:
        Camera3D();
        Camera3D(glm::vec3 position, glm::vec2 rotation);

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
    };
}