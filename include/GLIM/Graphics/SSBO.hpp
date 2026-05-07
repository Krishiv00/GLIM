#pragma once

#include <cstdint>

#include "GL/glew.h"

namespace gl {
    class SSBO {
    private:
        unsigned int m_Handle{0u};
        unsigned int m_Index{0u};

    public:
        SSBO() = default;
        SSBO(unsigned int index) : m_Index(index) {}

        ~SSBO() {
            if (m_Handle) glDeleteBuffers(1, &m_Handle);
        }

        SSBO(const SSBO&) = delete;
        SSBO& operator=(const SSBO&) = delete;

        SSBO(SSBO&& other) noexcept : m_Handle(other.m_Handle) {
            other.m_Handle = 0u;
        }

        SSBO& operator=(SSBO&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteBuffers(1, &m_Handle);

                m_Handle = other.m_Handle;
                other.m_Handle = 0u;
            }

            return *this;
        }

        void Upload(const void* data, std::size_t size) {
            if (m_Handle) glDeleteBuffers(1, &m_Handle);

            glGenBuffers(1, &m_Handle);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Index, m_Handle);
        }
    };
}