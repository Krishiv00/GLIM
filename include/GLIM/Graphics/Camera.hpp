#pragma once

#include "glm/glm.hpp"

#include "GLIM/System/Vector2.hpp"
#include "GLIM/System/Vector3.hpp"

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

        Vector2f m_Position{0.f, 0.f};
        Vector2f m_Size{1.f, 1.f};

        float m_Rotation{0.f};
        float m_Zoom{1.f};

    public:
        Camera2D();
        Camera2D(Vector2f position, Vector2f size);

        void Move(Vector2f delta);

        void Rotate(float delta);

        void Zoom(float delta);

        void SetPosition(Vector2f position);

        void SetSize(Vector2f size);

        void SetRotation(float rotation);

        void SetZoom(float zoom);

        [[nodiscard]]
        inline float GetRotation() const noexcept {
            return m_Rotation;
        }

        [[nodiscard]]
        inline Vector2f GetPosition() const noexcept {
            return m_Position;
        }

        [[nodiscard]]
        inline Vector2f GetSize() const noexcept {
            return m_Size;
        }

        [[nodiscard]]
        inline float GetZoom() const noexcept {
            return m_Zoom;
        }
    };

    class Camera3D : public Camera {
    public:
        static constexpr inline const Vector3f Up = Vector3f(0.f, 1.f, 0.f);
        static constexpr inline const float MaxPitch = glm::radians<float>(89.f);

    private:
        void clampRotation();
        void updateForward();
        void updateVpMatrix();

        Vector2f m_Rotation{0.f, 0.f};
        Vector3f m_Position{0.f, 0.f, 0.f};

        Vector3f m_Forward;

        float m_FOV{glm::radians<float>(60.f)};
        float m_Aspect{1.f};
        float m_Near{0.1f};
        float m_Far{1000.f};

    public:
        Camera3D();
        Camera3D(Vector3f position, Vector2f rotation);

        void Move(Vector3f delta);

        void Rotate(Vector2f delta);

        void SetPosition(Vector3f position);

        void SetRotation(Vector2f rotation);

        void SetFov(unsigned int fov);

        void SetAspect(float aspect);

        void SetClipping(float near, float far);

        [[nodiscard]]
        Vector3f GetRight() const;

        [[nodiscard]]
        Vector3f GetFront() const;

        [[nodiscard]]
        inline Vector2f GetRotation() const noexcept {
            return m_Rotation;
        }

        [[nodiscard]]
        inline Vector3f GetPosition() const noexcept {
            return m_Position;
        }

        [[nodiscard]]
        inline Vector3f GetForward() const noexcept {
            return m_Forward;
        }
    };
}