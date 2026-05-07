#pragma once

namespace gl {
    struct Color {
        constexpr Color() = default;
        constexpr Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1.f) {}
        constexpr Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

        float r{1.f}, g{1.f}, b{1.f}, a{1.f};

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Magenta;
        static const Color Cyan;
        static const Color Transparent;
    };

    inline constexpr Color Color::Black{0.f, 0.f, 0.f, 1.f};
    inline constexpr Color Color::White{1.f, 1.f, 1.f, 1.f};
    inline constexpr Color Color::Red{1.f, 0.f, 0.f, 1.f};
    inline constexpr Color Color::Green{0.f, 1.f, 0.f, 1.f};
    inline constexpr Color Color::Blue{0.f, 0.f, 1.f, 1.f};
    inline constexpr Color Color::Yellow{1.f, 1.f, 0.f, 1.f};
    inline constexpr Color Color::Magenta{1.f, 0.f, 1.f, 1.f};
    inline constexpr Color Color::Cyan{0.f, 1.f, 1.f, 1.f};
    inline constexpr Color Color::Transparent{0.f, 0.f, 0.f, 0.f};
}