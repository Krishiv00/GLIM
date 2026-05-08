#pragma once

#include <queue>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "GLIM/Graphics/Color.hpp"
#include "GLIM/Graphics/View.hpp"
#include "GLIM/Graphics/VideoMode.hpp"
#include "GLIM/System/Event.hpp"

#include "GLIM/System/Vector2.hpp"
#include "GLIM/System/Clock.hpp"

namespace gl {
    enum class Style {
        Default, Close
    };

    enum class State {
        Windowed, Fullscreen
    };

    class Window final {
    private:
        void pushEvent(Event event);

        void registerEventCallbacks();

        GLFWwindow* m_Handle{nullptr};

        // state
        bool m_Fullscreen{false};
        bool m_Vsync{false};

        // before fullscreen toggled
        Vector2u m_LastSize;
        Vector2i m_LastPosition;

        std::queue<Event> m_EventQueue;

        View m_View;

        float m_TargetFrameTime{0.f};

        Clock m_FrameClock;

    public:
        Window(
            unsigned int width, unsigned int height, const char* title,
            Style style = Style::Default, State state = State::Windowed
        );

        ~Window();

        Window() = default;
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void Create(
            unsigned int width, unsigned int height, const char* title,
            Style style = Style::Default, State state = State::Windowed
        );

        void SetSize(Vector2u size);

        void SetPosition(Vector2i position);

        void SetMetadata(const std::string& string);

        void SetTitle(const std::string& title);

        void SetMouseVisible(bool visible);

        void SetFramerateLimit(unsigned int fps);

        void SetVsync(bool vsync);

        void SetFullscreen(bool fullscreen);

        [[nodiscard]]
        std::optional<Event> PollEvents();

        void Clear(Color clearColor = Color::Black, bool clearDf = true);

        void Display();

        void Close();

        void SetView(const View& view);

        [[nodiscard]]
        Vector2u GetSize() const;

        [[nodiscard]]
        Vector2i GetPosition() const;

        [[nodiscard]]
        bool isOpen() const;

        [[nodiscard]]
        inline bool GetVsync() const noexcept {
            return m_Vsync;
        }

        [[nodiscard]]
        inline bool GetFullscreen() const noexcept {
            return m_Fullscreen;
        }

        [[nodiscard]]
        inline const View& GetView() const noexcept {
            return m_View;
        }

        operator GLFWwindow* () const {
            return m_Handle;
        }
    };
}