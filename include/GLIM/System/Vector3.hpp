#pragma once

#include <cassert>

namespace gl {
    template <typename T>
    struct Vector3 {
        Vector3() = default;
        Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

        template <typename U>
        Vector3(Vector3<U> vec)
            : x(static_cast<T>(vec.x)),
            y(static_cast<T>(vec.y)),
            z(static_cast<T>(vec.z)) {}

        Vector3<T> operator+(const Vector3<T>& rhs) const {
            return {x + rhs.x, y + rhs.y, z + rhs.z};
        }

        Vector3<T> operator-(const Vector3<T>& rhs) const {
            return {x - rhs.x, y - rhs.y, z - rhs.z};
        }

        Vector3<T>& operator+=(const Vector3<T>& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vector3<T>& operator-=(const Vector3<T>& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        Vector3<T> operator*(T scalar) const {
            return {x * scalar, y * scalar, z * scalar};
        }

        Vector3<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3<T> operator/(const Vector3<T>& rhs) const {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && "division by 0");
            return {x / rhs.x, y / rhs.y, z / rhs.z};
        }

        Vector3<T>& operator/=(const Vector3<T>& rhs) {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && "division by 0");
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            return *this;
        }

        Vector3<T> operator/(T scalar) const {
            assert(scalar != 0 && "division by 0");
            return {x / scalar, y / scalar, z / scalar};
        }

        Vector3<T>& operator/=(T scalar) {
            assert(scalar != 0 && "division by 0");
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        [[nodiscard]]
        bool operator==(const Vector3<T>& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        [[nodiscard]]
        bool operator!=(const Vector3<T>& rhs) const {
            return !(*this == rhs);
        }

        T x{T(0)}, y{T(0)}, z{T(0)};
    };

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;
    using Vector3u = Vector3<unsigned int>;
}