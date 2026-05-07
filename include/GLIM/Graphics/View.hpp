#pragma once

namespace gl {
    struct View {
        float x{0.f}, y{0.f};
        float width{0.f}, height{0.f};
    
        View() = default;
        View(float _x, float _y, float _width, float _height) : x(_x), y(_y), width(_width), height(_height) {}
    };
}