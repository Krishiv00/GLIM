#include "GLIM/Widgets/Textbox.hpp"

using namespace gl::Widgets;

[[nodiscard]]
inline bool IsWordChar(char c) noexcept {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

[[nodiscard]]
constexpr inline char GetOpening(char c) noexcept {
    if (c == '}') return '{';
    else if (c == ')') return '(';
    else if (c == ']') return '[';
    else return c;
}

[[nodiscard]]
constexpr inline char GetClosing(char c) noexcept {
    if (c == '{') return '}';
    else if (c == '(') return ')';
    else if (c == '[') return ']';
    else return c;
}

[[nodiscard]]
constexpr inline bool IsOpeningChar(char c) noexcept {
    return c == '{' || c == '(' || c == '[' || c == '"' || c == '\'';
}

[[nodiscard]]
constexpr inline bool IsClosingChar(char c) noexcept {
    return c == '}' || c == ')' || c == ']' || c == '"' || c == '\'';
}

std::size_t Textbox::findNextPosition(std::size_t position, int direction) const {
    // invalid direction arguement
    if (direction != 1 && direction != -1) return position;

    const std::size_t sz = m_Buffer.size();

    const auto peek = [&](std::size_t i) -> char { return m_Buffer[direction == 1 ? i : (i - 1)]; };

    // skip whitespace
    while (
        (direction == 1 ? (position < sz) : (position > 0)) &&
        std::isspace(static_cast<unsigned char>(peek(position)))
    ) {
        position += direction;
    }

    // out of bounds
    if (direction == 1 ? (position >= sz) : (position == 0)) return position;

    // match the character type
    // for word chars skip all consecutive word chars and vice versa
    const bool isWord = IsWordChar(peek(position));

    while ((direction == 1 ? (position < sz) : (position > 0))) {
        const char c = peek(position);

        // current character type is different from the starting one
        if (std::isspace(static_cast<unsigned char>(c)) || IsWordChar(c) != isWord) break;

        position += direction;
    }

    return position;
}

std::pair<std::size_t, std::size_t> Textbox::getSelectionRange() const {
    return std::make_pair(
        // left of selection
        std::min(m_CursorPosition, m_SelectionAnchor),
        // right of selection
        std::max(m_CursorPosition, m_SelectionAnchor)
    );
}

bool Textbox::hasSelection() const {
    return m_CursorPosition != m_SelectionAnchor;
}

void Textbox::deleteSelection() {
    if (!hasSelection()) return;

    const auto [start, end] = getSelectionRange();

    // remove text in selection range
    m_Buffer.erase(m_Buffer.begin() + start, m_Buffer.begin() + end);
    jumpCursorTo(start);
}

void Textbox::jumpCursorTo(std::size_t position) {
    m_CursorPosition = m_SelectionAnchor = position;
}

void Textbox::surroundSelection(char left, char right) {
    if (!hasSelection()) return;

    const auto [start, end] = getSelectionRange();

    m_Buffer.insert(m_Buffer.begin() + end, right);
    m_Buffer.insert(m_Buffer.begin() + start, left);

    m_CursorPosition = end + 2;
    m_SelectionAnchor = start + 1;
}

void Textbox::handleKeyEvent(Keyboard::Scancode key, bool controlPressed, bool shiftPressed) {
    m_LastEnteredChar = '\0';

    if (key == Keyboard::Scancode::Backspace) {
        if (hasSelection()) {
            deleteSelection();
        }

        else if (m_CursorPosition > 0) {
            // delete entire word to the left when control is pressed
            if (controlPressed) {
                const std::size_t nextPosition = findNextPosition(m_CursorPosition, -1);

                m_Buffer.erase(m_Buffer.begin() + nextPosition, m_Buffer.begin() + m_CursorPosition);
                m_CursorPosition = nextPosition;
            }

            // remove only 1 character to the left
            else {
                m_Buffer.erase(m_Buffer.begin() + --m_CursorPosition);
            }

            m_SelectionAnchor = m_CursorPosition;
        }
    }

    else if (key == Keyboard::Scancode::Delete) {
        if (hasSelection()) {
            deleteSelection();
        }

        else if (m_CursorPosition < m_Buffer.size()) {
            // delete entire word to the right when control is pressed
            if (controlPressed) {
                m_Buffer.erase(
                    m_Buffer.begin() + m_CursorPosition,
                    m_Buffer.begin() + findNextPosition(m_CursorPosition, 1)
                );
            }

            // remove only 1 character to the right
            else {
                m_Buffer.erase(m_Buffer.begin() + m_CursorPosition);
            }
        }
    }

    else if (key == Keyboard::Scancode::Left) {
        // move main cursor
        if (!shiftPressed && hasSelection()) {
            m_CursorPosition = getSelectionRange().first;
        } else if (controlPressed) {
            m_CursorPosition = findNextPosition(m_CursorPosition, -1);
        } else if (m_CursorPosition > 0) {
            --m_CursorPosition;
        }

        // select when shift is pressed
        if (!shiftPressed) m_SelectionAnchor = m_CursorPosition;
    }

    else if (key == Keyboard::Scancode::Right) {
        // move main cursor
        if (!shiftPressed && hasSelection()) {
            m_CursorPosition = getSelectionRange().second;
        } else if (controlPressed) {
            m_CursorPosition = findNextPosition(m_CursorPosition, 1);
        } else if (m_CursorPosition < m_Buffer.size()) {
            ++m_CursorPosition;
        }

        // select when shift is pressed
        if (!shiftPressed) m_SelectionAnchor = m_CursorPosition;
    }

    else if (key == Keyboard::Scancode::Home) {
        // move cursor to the start of the line
        m_CursorPosition = 0;
        if (!shiftPressed) m_SelectionAnchor = m_CursorPosition;
    }

    else if (key == Keyboard::Scancode::End) {
        // move cursor to end of the line
        m_CursorPosition = m_Buffer.size();
        if (!shiftPressed) m_SelectionAnchor = m_CursorPosition;
    }

    else if (key == Keyboard::Scancode::A) {
        if (controlPressed) {
            // select all
            m_CursorPosition = m_Buffer.size();
            m_SelectionAnchor = 0;
        }
    }
}

void Textbox::handleTextEvent(char c) {
    if (
        // invalid ASCII character
        c < 32 ||
        // backspace (handled in HandleKeyEvent)
        c == 127
    ) return;

    if (hasSelection()) {
        // surround selecition with parenthesis or quotes
        if (IsOpeningChar(c)) {
            surroundSelection(c, GetClosing(c));
            // end the routine here
            return;
        }

        // clear existing selection before writing
        else {
            deleteSelection();
        }
    }

    if (
        // skip subsequent closing char after opening, just move the cursor
        // for example: skip typing closing paren when '(' and ')' are typed simultaneously
        IsClosingChar(c) &&
        // last typed character was the opening one
        m_LastEnteredChar == GetOpening(c) &&
        // char under cursor is same as this char
        m_CursorPosition < m_Buffer.size() && m_Buffer[m_CursorPosition] == c
    ) {
        jumpCursorTo(m_CursorPosition + 1);
    }

    // type the char normally
    else {
        m_Buffer.insert(m_Buffer.begin() + m_CursorPosition, c);
        jumpCursorTo(m_CursorPosition + 1);

        // auto parenthesis & auto quote
        if (IsOpeningChar(c)) {
            m_Buffer.insert(m_Buffer.begin() + m_CursorPosition, GetClosing(c));
        }
    }

    // store this char for future checks
    m_LastEnteredChar = c;
}

void Textbox::HandleEvent(const gl::Event& event) {
    if (const auto* key = event.getIf<gl::Event::KeyPressed>()) {
        handleKeyEvent(key->Scancode, key->ModControl, key->ModShift);
    } else if (const auto* text = event.getIf<gl::Event::TextEntered>()) {
        handleTextEvent(static_cast<char>(text->Unicode));
    }
}

void Textbox::Copy() const {
    if (!hasSelection()) return;

    gl::Clipboard::CopyString(GetSelected());
}

void Textbox::Cut() {
    if (!hasSelection()) return;

    gl::Clipboard::CopyString(GetSelected());
    deleteSelection();
}

void Textbox::Paste(const std::string& string) {
    if (string.empty()) return;

    // delete selected text (if any)
    deleteSelection();

    m_Buffer.insert(m_Buffer.begin() + m_CursorPosition, string.begin(), string.end());
    jumpCursorTo(m_CursorPosition + string.size());

    m_LastEnteredChar = '\0';
}

void Textbox::Clear() {
    jumpCursorTo(0);
    m_LastEnteredChar = '\0';
    m_Buffer.clear();
}

std::string Textbox::GetSelected() const {
    if (!hasSelection()) return "";

    const auto [start, end] = getSelectionRange();
    return std::string{m_Buffer.begin() + start, m_Buffer.begin() + end};
}
