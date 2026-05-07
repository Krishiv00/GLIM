#include "GLIM/Graphics/VideoMode.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace gl;

VideoMode VideoMode::GetDesktopMode() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) return VideoMode();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) return VideoMode();

    return {
        .Size = Vector2u(static_cast<unsigned int>(mode->width), static_cast<unsigned int>(mode->height)),
        .RefreshRate = static_cast<unsigned int>(mode->refreshRate)
    };
}