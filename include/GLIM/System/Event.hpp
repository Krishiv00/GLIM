#pragma once

#include <optional>
#include <variant>
#include <type_traits>

#include "GLIM/System/Vector2.hpp"
#include "GLIM/System/Input.hpp"

namespace gl {
    class Event {
    public:
        struct Resized {
            Vector2u Size;
        };

        struct KeyPressed {
            Keyboard::Scancode Scancode;
            bool ModAlt{false}, ModControl{false}, ModShift{false}, ModSystem{false};
        };

        struct KeyReleased {
            Keyboard::Scancode Scancode;
            bool ModAlt{false}, ModControl{false}, ModShift{false}, ModSystem{false};
        };

        struct TextEntered {
            char32_t Unicode{0};
        };

        struct MouseButtonPressed {
            Mouse::Button Button;
            Vector2i Position;
        };

        struct MouseButtonReleased {
            Mouse::Button Button;
            Vector2i Position;
        };

        struct MouseWheelScrolled {
            float Delta{0.f};
            Vector2i Position;
        };

        struct MouseMoved {
            Vector2i Position;
        };

        struct MouseEntered {};
        struct MouseLeft {};

        struct FocusGained {};
        struct FocusLost {};

    private:
        using variant_t = std::variant<
            Resized,
            KeyPressed, KeyReleased, TextEntered,
            MouseButtonPressed, MouseButtonReleased, MouseWheelScrolled, MouseMoved,
            MouseEntered, MouseLeft,
            FocusGained, FocusLost
        >;

        variant_t m_Data;

    public:
        template <typename T>
            requires (!std::same_as<std::remove_cvref_t<T>, Event>)
        Event(T&& data) : m_Data(std::forward<T>(data)) {}

        template <typename T>
        [[nodiscard]]
        bool is() const noexcept {
            return std::holds_alternative<std::remove_cvref_t<T>>(m_Data);
        }

        template <typename T>
        [[nodiscard]]
        const std::remove_cvref_t<T>* getIf() const noexcept {
            return std::get_if<std::remove_cvref_t<T>>(&m_Data);
        }
    };
}