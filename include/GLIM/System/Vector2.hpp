#pragma once

#include <cassert>

namespace gl {
    template <typename T>
    struct Vector2 {
        Vector2() = default;
        Vector2(T _x, T _y) : x(_x), y(_y) {}

        template <typename U>
        Vector2(Vector2<U> vec) : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

        Vector2<T> operator+(const Vector2<T>& rhs) const {
            return {x + rhs.x, y + rhs.y};
        }

        Vector2<T> operator-(const Vector2<T>& rhs) const {
            return {x - rhs.x, y - rhs.y};
        }

        Vector2<T>& operator+=(const Vector2<T>& rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Vector2<T>& operator-=(const Vector2<T>& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        Vector2<T> operator*(T scalar) const {
            return {x * scalar, y * scalar};
        }

        Vector2<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2<T> operator/(const Vector2<T>& rhs) const {
            assert(rhs.x != 0 && rhs.y != 0 && "division by 0");
            return {x / rhs.x, y / rhs.y};
        }

        Vector2<T>& operator/=(const Vector2<T>& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && "division by 0");
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }

        Vector2<T> operator/(T scalar) const {
            assert(scalar != 0 && "division by 0");
            return {x / scalar, y / scalar};
        }

        Vector2<T>& operator/=(T scalar) {
            assert(scalar != 0 && "division by 0");
            x /= scalar;
            y /= scalar;
            return *this;
        }

        [[nodiscard]]
        bool operator==(const Vector2<T>& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        [[nodiscard]]
        bool operator!=(const Vector2<T>& rhs) const {
            return !(*this == rhs);
        }

        T x{T(0)}, y{T(0)};
    };

    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;
    using Vector2u = Vector2<unsigned int>;
}