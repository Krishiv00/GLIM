#pragma once

#include <string>

#include "GLIM/System/Input.hpp"
#include "GLIM/System/Event.hpp"
#include "GLIM/System/Clipboard.hpp"

namespace gl::Widgets {
    class Textbox {
    private:
        std::size_t findNextPosition(std::size_t position, int direction) const;

        std::pair<std::size_t, std::size_t> getSelectionRange() const;
        bool hasSelection() const;
        void deleteSelection();
        void jumpCursorTo(std::size_t position);
        void surroundSelection(char left, char right);

        void handleKeyEvent(Keyboard::Scancode key, bool controlPressed, bool shiftPressed);
        void handleTextEvent(char c);

        std::string m_Buffer;
        std::size_t m_CursorPosition{0}, m_SelectionAnchor{0};

        char m_LastEnteredChar{'\0'};

    public:
        void HandleEvent(const gl::Event& event);

        void Copy() const;
        void Cut();
        void Paste(const std::string& string);

        void Clear();

        [[nodiscard]]
        std::string GetSelected() const;

        [[nodiscard]]
        inline const std::string& GetString() const noexcept {
            return m_Buffer;
        }
    };
}
