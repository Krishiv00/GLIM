#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "GLIM/System/Vector2.hpp"

namespace gl {
    class Keyboard final {
        friend class Window;

    private:
        static void init(GLFWwindow* window);
        static void update();

        static constexpr int KEY_COUNT = GLFW_KEY_LAST + 1;

        static inline bool s_Current[KEY_COUNT]{};
        static inline bool s_Previous[KEY_COUNT]{};
        static inline bool s_Repeat[KEY_COUNT]{};

        static inline bool s_DoRepeat{true};

    public:
        [[nodiscard]]
        static bool IsKeyDown(int key);

        [[nodiscard]]
        static bool IsKeyPressed(int key);

        [[nodiscard]]
        static bool IsKeyReleased(int key);

        [[nodiscard]]
        static bool IsKeyTyped(int key);

        static void SetKeyRepeat(bool repeat);
    };

    class Mouse final {
        friend class Window;

    public:
        enum class Button {
            Left = GLFW_MOUSE_BUTTON_LEFT,
            Right = GLFW_MOUSE_BUTTON_RIGHT,
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,
            Button4 = GLFW_MOUSE_BUTTON_4,
            Button5 = GLFW_MOUSE_BUTTON_5,
            Button6 = GLFW_MOUSE_BUTTON_6,
            Button7 = GLFW_MOUSE_BUTTON_7,
            Button8 = GLFW_MOUSE_BUTTON_8
        };

    private:
        static void init(GLFWwindow* window);
        static void update();

        static constexpr int BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST - GLFW_MOUSE_BUTTON_LEFT + 1;

        static inline bool s_Current[BUTTON_COUNT]{};
        static inline bool s_Previous[BUTTON_COUNT]{};

        static inline double s_PosX = 0.0, s_PosY = 0.0;
        static inline double s_PrevPosX = 0.0, s_PrevPosY = 0.0;

    public:
        static bool IsButtonDown(Button button);
        static bool IsButtonPressed(Button button);
        static bool IsButtonReleased(Button button);

        static bool IsMoved();

        static void SetPosition(GLFWwindow* window, Vector2<double> position);

        [[nodiscard]]
        static Vector2<double> GetPosition();

        [[nodiscard]]
        static Vector2<double> GetDelta();
    };
}