#pragma once

#include <cassert>
#include <cmath>

namespace gl {
    template <typename T>
    struct Vector2 {
        constexpr Vector2() = default;
        constexpr Vector2(T _x, T _y) : x(_x), y(_y) {}

        template <typename U>
        constexpr Vector2(const Vector2<U>& vec) : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

        [[nodiscard]]
        constexpr Vector2<T> operator+(const Vector2<T>& rhs) const {
            return {x + rhs.x, y + rhs.y};
        }

        [[nodiscard]]
        constexpr Vector2<T> operator-(const Vector2<T>& rhs) const {
            return {x - rhs.x, y - rhs.y};
        }

        [[nodiscard]]
        constexpr Vector2<T> operator-() const {
            return {-x, -y};
        }

        constexpr Vector2<T>& operator+=(const Vector2<T>& rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr Vector2<T>& operator-=(const Vector2<T>& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        [[nodiscard]]
        constexpr Vector2<T> operator*(T scalar) const {
            return {x * scalar, y * scalar};
        }

        [[nodiscard]]
        friend constexpr Vector2 operator*(T scalar, const Vector2& vec) {
            return {vec.x * scalar, vec.y * scalar};
        }

        constexpr Vector2<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        [[nodiscard]]
        constexpr Vector2<T> operator/(const Vector2<T>& rhs) const {
            assert(rhs.x != 0 && rhs.y != 0 && "division by 0");
            return {x / rhs.x, y / rhs.y};
        }

        constexpr Vector2<T>& operator/=(const Vector2<T>& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && "division by 0");
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }

        [[nodiscard]]
        constexpr Vector2<T> operator/(T scalar) const {
            assert(scalar != 0 && "division by 0");
            return {x / scalar, y / scalar};
        }

        constexpr Vector2<T>& operator/=(T scalar) {
            assert(scalar != 0 && "division by 0");
            x /= scalar;
            y /= scalar;
            return *this;
        }

        [[nodiscard]]
        constexpr bool operator==(const Vector2<T>& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        [[nodiscard]]
        constexpr bool operator!=(const Vector2<T>& rhs) const {
            return !(*this == rhs);
        }

        [[nodiscard]]
        constexpr T Dot(const Vector2<T>& rhs) const {
            return x * rhs.x + y * rhs.y;
        }

        [[nodiscard]]
        constexpr T Cross(const Vector2<T>& rhs) const {
            return x * rhs.y - y * rhs.x;
        }

        [[nodiscard]]
        T LengthSquared() const {
            return x * x + y * y;
        }

        [[nodiscard]]
        T Length() const {
            return std::sqrt(LengthSquared());
        }

        [[nodiscard]]
        Vector2<T> Normalize() const {
            const T len = Length();

            assert(len != 0 && "normalizing zero vector");

            return {
                x / len,
                y / len
            };
        }

        T x{T(0)}, y{T(0)};
    };

    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;
    using Vector2u = Vector2<unsigned int>;
}