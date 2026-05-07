#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace gl {
#pragma region Types

    template<typename T>
    struct TypeTraits;

    template<>
    struct TypeTraits<float> {
        static constexpr inline const unsigned int Type = GL_FLOAT;
    };

    template<>
    struct TypeTraits<unsigned int> {
        static constexpr inline const unsigned int Type = GL_UNSIGNED_INT;
    };

    template<>
    struct TypeTraits<unsigned char> {
        static constexpr inline const unsigned int Type = GL_UNSIGNED_BYTE;
    };

#pragma region Uniforms

    template<typename T, std::size_t N>
    struct UniformTraits;

    template<>
    struct UniformTraits<float, 1> {
        static void Set(GLint loc, float v0) {
            glUniform1f(loc, v0);
        }
    };

    template<>
    struct UniformTraits<float, 2> {
        static void Set(GLint loc, float v0, float v1) {
            glUniform2f(loc, v0, v1);
        }
    };

    template<>
    struct UniformTraits<float, 3> {
        static void Set(GLint loc, float v0, float v1, float v2) {
            glUniform3f(loc, v0, v1, v2);
        }
    };

    template<>
    struct UniformTraits<float, 4> {
        static void Set(GLint loc, float v0, float v1, float v2, float v3) {
            glUniform4f(loc, v0, v1, v2, v3);
        }
    };

    template<>
    struct UniformTraits<int, 1> {
        static void Set(GLint loc, int v0) {
            glUniform1i(loc, v0);
        }
    };

    template<>
    struct UniformTraits<int, 2> {
        static void Set(GLint loc, int v0, int v1) {
            glUniform2i(loc, v0, v1);
        }
    };

    template<>
    struct UniformTraits<int, 3> {
        static void Set(GLint loc, int v0, int v1, int v2) {
            glUniform3i(loc, v0, v1, v2);
        }
    };

    template<>
    struct UniformTraits<int, 4> {
        static void Set(GLint loc, int v0, int v1, int v2, int v3) {
            glUniform4i(loc, v0, v1, v2, v3);
        }
    };

    template<>
    struct UniformTraits<unsigned int, 1> {
        static void Set(GLint loc, unsigned int v0) {
            glUniform1ui(loc, v0);
        }
    };

    template<>
    struct UniformTraits<unsigned int, 2> {
        static void Set(GLint loc, unsigned int v0, unsigned int v1) {
            glUniform2ui(loc, v0, v1);
        }
    };

    template<>
    struct UniformTraits<unsigned int, 3> {
        static void Set(GLint loc, unsigned int v0, unsigned int v1, unsigned int v2) {
            glUniform3ui(loc, v0, v1, v2);
        }
    };

    template<>
    struct UniformTraits<unsigned int, 4> {
        static void Set(GLint loc, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) {
            glUniform4ui(loc, v0, v1, v2, v3);
        }
    };

    template<>
    struct UniformTraits<glm::vec3, 1> {
        static void Set(GLint loc, glm::vec3 vec) {
            glUniform3f(loc, vec.x, vec.y, vec.z);
        }
    };

    template<>
    struct UniformTraits<glm::mat4, 1> {
        static void Set(GLint loc, const glm::mat4& mat) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        }
    };

#pragma region Primitives

    enum class PrimitiveType {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN
    };
}