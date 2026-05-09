#pragma once

#include <cassert>
#include <cmath>

namespace gl {
    template <typename T>
    struct Vector3 {
        constexpr Vector3() = default;

        constexpr Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

        template <typename U>
        constexpr Vector3(const Vector3<U>& vec)
            : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)), z(static_cast<T>(vec.z)) {}

        [[nodiscard]]
        constexpr Vector3<T> operator+(const Vector3<T>& rhs) const {
            return {x + rhs.x, y + rhs.y, z + rhs.z};
        }

        [[nodiscard]]
        constexpr Vector3<T> operator-(const Vector3<T>& rhs) const {
            return {x - rhs.x, y - rhs.y, z - rhs.z};
        }

        [[nodiscard]]
        constexpr Vector3<T> operator-() const {
            return {-x, -y, -z};
        }

        constexpr Vector3<T>& operator+=(const Vector3<T>& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        constexpr Vector3<T>& operator-=(const Vector3<T>& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        [[nodiscard]]
        constexpr Vector3<T> operator*(T scalar) const {
            return {x * scalar, y * scalar, z * scalar};
        }

        [[nodiscard]]
        friend constexpr Vector3 operator*(T scalar, const Vector3& vec) {
            return {vec.x * scalar, vec.y * scalar, vec.z * scalar};
        }

        constexpr Vector3<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        [[nodiscard]]
        constexpr Vector3<T> operator/(const Vector3<T>& rhs) const {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && "division by 0");
            return {x / rhs.x, y / rhs.y, z / rhs.z};
        }

        constexpr Vector3<T>& operator/=(const Vector3<T>& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && "division by 0");
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            return *this;
        }

        [[nodiscard]]
        constexpr Vector3<T> operator/(T scalar) const {
            assert(scalar != 0 && "division by 0");
            return {x / scalar, y / scalar, z / scalar};
        }

        constexpr Vector3<T>& operator/=(T scalar) {
            assert(scalar != 0 && "division by 0");
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        constexpr bool operator==(const Vector3<T>& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        constexpr bool operator!=(const Vector3<T>& rhs) const {
            return !(*this == rhs);
        }

        [[nodiscard]]
        constexpr T Dot(const Vector3<T>& rhs) const {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        [[nodiscard]]
        constexpr Vector3<T> Cross(const Vector3<T>& rhs) const {
            return {
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            };
        }

        [[nodiscard]]
        T LengthSquared() const {
            return x * x + y * y + z * z;
        }

        [[nodiscard]]
        T Length() const {
            return std::sqrt(LengthSquared());
        }

        [[nodiscard]]
        Vector3<T> Normalize() const {
            const T len = Length();

            assert(len != 0 && "normalizing zero vector");

            return {
                x / len,
                y / len,
                z / len
            };
        }

        T x{T(0)}, y{T(0)}, z{T(0)};
    };

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;
    using Vector3u = Vector3<unsigned int>;
}