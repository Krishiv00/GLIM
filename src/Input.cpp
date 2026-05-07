#include <cassert>
#include <cstring>

#include "GLIM/System/Input.hpp"

using namespace gl;

#pragma region Keyboard

void Keyboard::init(GLFWwindow* window) {
    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int action, int) {
        if (key < 0 || key >= KEY_COUNT) return;

        if (action == GLFW_PRESS) {
            s_Current[key] = true;
            s_Repeat[key] = true;
        }

        else if (action == GLFW_RELEASE) {
            s_Current[key] = false;
        }

        else if (action == GLFW_REPEAT) {
            s_Repeat[key] = s_DoRepeat;
        }
    });
}

void Keyboard::update() {
    std::memcpy(s_Previous, s_Current, sizeof(s_Current));
    std::memset(s_Repeat, 0, sizeof(s_Repeat));
}

bool Keyboard::IsKeyDown(int key) {
    assert(key >= 0 && key < KEY_COUNT);
    return s_Current[key];
}

bool Keyboard::IsKeyPressed(int key) {
    assert(key >= 0 && key < KEY_COUNT);
    return s_Current[key] && !s_Previous[key];
}

bool Keyboard::IsKeyReleased(int key) {
    assert(key >= 0 && key < KEY_COUNT);
    return !s_Current[key] && s_Previous[key];
}

bool Keyboard::IsKeyTyped(int key) {
    assert(key >= 0 && key < KEY_COUNT);
    return s_Repeat[key];
}

void Keyboard::SetKeyRepeat(bool repeat) {
    s_DoRepeat = repeat;
}

#pragma region Mouse

void Mouse::init(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int) {
        if (button >= 0 && button < BUTTON_COUNT) s_Current[button] = action == GLFW_PRESS;
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow*, double x, double y) {
        s_PosX = x;
        s_PosY = y;
    });
}

void Mouse::update() {
    std::memcpy(s_Previous, s_Current, sizeof(s_Current));

    s_PrevPosX = s_PosX;
    s_PrevPosY = s_PosY;
}

bool Mouse::IsButtonDown(Button button) {
    return s_Current[static_cast<int>(button)];
}

bool Mouse::IsButtonPressed(Button button) {
    return s_Current[static_cast<int>(button)] && !s_Previous[static_cast<int>(button)];
}

bool Mouse::IsButtonReleased(Button button) {
    return !s_Current[static_cast<int>(button)] && s_Previous[static_cast<int>(button)];
}

bool Mouse::IsMoved() {
    return s_PosX != s_PrevPosX || s_PosY != s_PrevPosY;
}

void Mouse::SetPosition(GLFWwindow* window, Vector2<double> position) {
    glfwSetCursorPos(window, position.x, position.y);

    s_PosX = s_PrevPosX = position.x;
    s_PosY = s_PrevPosY = position.y;
}

Vector2<double> Mouse::GetPosition() {
    return Vector2<double>(s_PosX, s_PosY);
}

Vector2<double> Mouse::GetDelta() {
    return Vector2<double>(s_PosX - s_PrevPosX, s_PrevPosY - s_PosY);
}