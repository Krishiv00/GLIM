#include <cassert>
#include <stdexcept>
#include <thread>

#include "GLIM/Graphics/Window.hpp"
#include "GLIM/System/Input.hpp"

using namespace gl;

namespace Helpers {
    [[nodiscard]]
    constexpr inline bool ModAlt(int mods) noexcept {
        return (mods & GLFW_MOD_ALT) != 0;
    }

    [[nodiscard]]
    constexpr inline bool ModControl(int mods) noexcept {
        return (mods & GLFW_MOD_CONTROL) != 0;
    }

    [[nodiscard]]
    constexpr inline bool ModShift(int mods) noexcept {
        return (mods & GLFW_MOD_SHIFT) != 0;
    }

    [[nodiscard]]
    constexpr inline bool ModSuper(int mods) noexcept {
        return (mods & GLFW_MOD_SUPER) != 0;
    }
}

void Window::pushEvent(Event event) {
    m_EventQueue.push(std::move(event));
}

void gl::Window::registerEventCallbacks() {
    assert(m_Handle && "window not created yet");

    glfwSetWindowUserPointer(m_Handle, this);

    // resize
    glfwSetFramebufferSizeCallback(m_Handle, [](GLFWwindow* w, int width, int height) {
        glViewport(0, 0, width, height);

        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));

        self->pushEvent(Event::Resized(Vector2u(
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height)
        )));
    });

    // focus lost / gained
    glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* w, int focused) {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));

        if (focused) self->pushEvent(Event::FocusGained());
        else self->pushEvent(Event::FocusLost());
    });

    // keyboard
    glfwSetKeyCallback(m_Handle, [](GLFWwindow* w, int key, int /*scancode*/, int action, int mods) {
        if (key < 0) return;

        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));

        if (action == GLFW_PRESS) {
            if (key < Keyboard::KEY_COUNT) {
                Keyboard::s_States[key] = true;
            }

            self->pushEvent(Event::KeyPressed(
                static_cast<Keyboard::Scancode>(key),
                Helpers::ModAlt(mods), Helpers::ModControl(mods),
                Helpers::ModShift(mods), Helpers::ModSuper(mods)
            ));
        }

        else if (action == GLFW_RELEASE) {
            if (key < Keyboard::KEY_COUNT) Keyboard::s_States[key] = false;

            self->pushEvent(Event::KeyReleased(
                static_cast<Keyboard::Scancode>(key),
                Helpers::ModAlt(mods), Helpers::ModControl(mods),
                Helpers::ModShift(mods), Helpers::ModSuper(mods)
            ));
        }
});

    // text / unicode input
    glfwSetCharCallback(m_Handle, [](GLFWwindow* w, unsigned int codepoint) {
        static_cast<Window*>(glfwGetWindowUserPointer(w))->pushEvent(Event::TextEntered(
            static_cast<char32_t>(codepoint)
        ));
    });

    // cursor movement
    glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* w, double x, double y) {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->pushEvent(Event::MouseMoved(Mouse::GetPosition(self->m_Handle)));
    });

    // cursor enter / leave
    glfwSetCursorEnterCallback(m_Handle, [](GLFWwindow* w, int entered) {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));

        if (entered) self->pushEvent(Event::MouseEntered());
        else self->pushEvent(Event::MouseLeft());
    });

    // mouse buttons
    glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* w, int button, int action, int /*mods*/) {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));

        if (button >= 0 && button < Mouse::BUTTON_COUNT) {
            Mouse::s_States[button] = action == GLFW_PRESS;
        }

        const Vector2i pos = Mouse::GetPosition(self->m_Handle);

        if (action == GLFW_PRESS) {
            self->pushEvent(Event::MouseButtonPressed(static_cast<Mouse::Button>(button), pos));
        } else if (action == GLFW_RELEASE) {
            self->pushEvent(Event::MouseButtonReleased(static_cast<Mouse::Button>(button), pos));
        }
    });

    // scroll wheel
    glfwSetScrollCallback(m_Handle, [](GLFWwindow* w, double /*xoffset*/, double yoffset) {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(w));

        self->pushEvent(Event::MouseWheelScrolled(
            static_cast<float>(yoffset), Mouse::GetPosition(self->m_Handle)
        ));
    });
}

Window::Window(
    unsigned int width, unsigned int height, const char* title,
    Style style, State state
) {
    Create(width, height, title, style, state);
}

Window::~Window() {
    if (m_Handle) {
        glfwDestroyWindow(m_Handle);
        glfwTerminate();
    }
}

void Window::Create(
    unsigned int width, unsigned int height, const char* title,
    Style style, State state
) {
    // init glfw
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // resizability
    glfwWindowHint(GLFW_RESIZABLE, style != Style::Close);

    m_Handle = glfwCreateWindow(
        width, height, title,
        state == State::Fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr
    );

    if (!m_Handle) {
        glfwTerminate();
        throw std::runtime_error("failed to create window");
    }

    m_View = View(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));

    m_Fullscreen = state == State::Fullscreen;

    const Vector2u desktopSize = VideoMode::GetDesktopMode().Size;
    const Vector2u defaultSize = Vector2f(desktopSize) / 1.5f;

    // size in case the window begin with fullscreen mode
    // in that case use some defaults
    m_LastSize = defaultSize;
    m_LastPosition = (desktopSize - defaultSize) / 2;

    // center on primary monitor
    SetPosition(Vector2i(
        (desktopSize.x - width) / 2,
        (desktopSize.y - height) / 2
    ));

    glfwMakeContextCurrent(m_Handle);

    registerEventCallbacks();

    // init glew
    glewInit();

    // configure
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::SetSize(Vector2u size) {
    assert(m_Handle && "window not created yet");
    glfwSetWindowSize(m_Handle, static_cast<int>(size.x), static_cast<int>(size.y));
}

void Window::SetPosition(Vector2i position) {
    assert(m_Handle && "window not created yet");
    glfwSetWindowPos(m_Handle, position.x, position.y);
}

void Window::SetMetadata(const std::string& metadata) {
    std::string title = glfwGetWindowTitle(m_Handle);

    std::size_t separatorPos = title.find(" | ");

    if (separatorPos != std::string::npos) {
        title = title.substr(0, separatorPos);
    }

    title += " | " + metadata;

    SetTitle(title);
}

void Window::SetTitle(const std::string& title) {
    glfwSetWindowTitle(m_Handle, title.c_str());
}

void Window::SetMouseVisible(bool visible) {
    glfwSetInputMode(
        m_Handle, GLFW_CURSOR,
        visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN
    );
}

void Window::SetFramerateLimit(unsigned int fps) {
    m_TargetFrameTime = fps ? (1.f / static_cast<float>(fps)) : 0.f;
}

void Window::SetVsync(bool vsync) {
    m_Vsync = vsync;
    glfwSwapInterval(vsync);
}

void Window::SetFullscreen(bool fullscreen) {
    if (fullscreen == m_Fullscreen) return;

    if (fullscreen) {
        m_LastSize = GetSize();
        m_LastPosition = GetPosition();

        const VideoMode desktopMode = VideoMode::GetDesktopMode();

        glfwSetWindowMonitor(
            m_Handle, glfwGetPrimaryMonitor(), 0, 0,
            desktopMode.Size.x, desktopMode.Size.y,
            desktopMode.RefreshRate
        );
    } else {
        glfwSetWindowMonitor(
            m_Handle, nullptr, m_LastPosition.x, m_LastPosition.y,
            static_cast<int>(m_LastSize.x), static_cast<int>(m_LastSize.y),
            0
        );

        m_View = View(
            0.f, 0.f,
            static_cast<float>(m_LastSize.x),
            static_cast<float>(m_LastSize.y)
        );
    }

    m_Fullscreen = fullscreen;

    glfwSwapInterval(m_Vsync);
}

std::optional<Event> Window::PollEvents() {
    // start polling events
    if (m_EventQueue.empty()) glfwPollEvents();

    // still empty, return
    if (m_EventQueue.empty()) return std::nullopt;

    const Event e = std::move(m_EventQueue.front());
    m_EventQueue.pop();

    return e;
}

void Window::Clear(Color clearColor, bool clearDf) {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearDf ? (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) : GL_COLOR_BUFFER_BIT);
}

void Window::Display() {
    assert(m_Handle && "window not created yet");

    glfwSwapBuffers(m_Handle);

    if (m_TargetFrameTime > 0.f) {
        const float sleepUntil = m_TargetFrameTime - 0.02f;

        if (m_FrameClock.GetElapsedTime() < sleepUntil) {
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepUntil - m_FrameClock.GetElapsedTime()));
        }

        while (m_FrameClock.GetElapsedTime() < m_TargetFrameTime);
    }

    m_FrameClock.Restart();
}

void Window::Close() {
    if (m_Handle) glfwSetWindowShouldClose(m_Handle, true);
}

void Window::SetView(const View& view) {
    m_View = view;
}

Vector2u Window::GetSize() const {
    assert(m_Handle && "window not created yet");

    int w, h;
    glfwGetWindowSize(m_Handle, &w, &h);

    return Vector2i(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
}

Vector2i Window::GetPosition() const {
    assert(m_Handle && "window not created yet");

    int x, y;
    glfwGetWindowPos(m_Handle, &x, &y);

    return Vector2i(x, y);
}

bool Window::isOpen() const {
    assert(m_Handle && "window not created yet");
    return !glfwWindowShouldClose(m_Handle);
}