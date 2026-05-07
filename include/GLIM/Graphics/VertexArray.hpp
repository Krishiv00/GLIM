#pragma once

#include <vector>
#include <type_traits>
#include <cassert>

#include "GL/glew.h"

#include "GLIM/Graphics/TypeTraits.hpp"

namespace gl {
    class VertexArray final {
    private:
        struct Element {
            unsigned int Count;
            unsigned int Type;
            unsigned int Offset;
            bool Normalized;
        };

        unsigned int m_VAO{0};
        unsigned int m_VBO{0};

        std::vector<Element> m_Layout;
        std::vector<uint8_t> m_Data;

        unsigned int m_Stride{0};

        unsigned int m_PrimitiveType{GL_TRIANGLES};

        mutable bool m_Dirty{true};

    public:
        VertexArray(gl::PrimitiveType primitiveType);

        ~VertexArray();

        VertexArray() = default;
        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        template<typename T>
        void PushLayout(unsigned int count, bool normalized = false) {
            if (!m_VAO) glGenVertexArrays(1, &m_VAO);
            if (!m_VBO) glGenBuffers(1, &m_VBO);

            m_Layout.push_back({
                .Count = count,
                .Type = gl::TypeTraits<T>::Type,
                .Offset = m_Stride,
                .Normalized = normalized
            });

            m_Stride += count * sizeof(T);
        }

        template<typename... Args>
        void Append(Args... args) {
            static_assert((std::is_trivially_copyable_v<Args> && ...), "append only supports trivially copyable types");
            assert(m_Stride && "vertex layout must be defined before appending");

            ([&] {
                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&args);
                m_Data.insert(m_Data.end(), ptr, ptr + sizeof(args));
            }(), ...);

            m_Dirty = true;
        }

        void SetPrimitiveType(gl::PrimitiveType type);

        void Clear();

        void Upload() const;

        void Draw() const;

        unsigned int GetVertexCount() const;
    };
}