#pragma once

#include <filesystem>

#include "GL/glew.h"

#include "glm/glm.hpp"

namespace gl {
    class Texture {
    private:
        unsigned int m_Handle{0u};

    public:
        ~Texture();

        Texture() = default;
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept : m_Handle(other.m_Handle) {
            other.m_Handle = 0u;
        }

        Texture& operator=(Texture&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteTextures(1, &m_Handle);

                m_Handle = other.m_Handle;
                other.m_Handle = 0u;
            }

            return *this;
        }

        void Bind() const;

        [[nodiscard]]
        bool LoadFromFile(const std::filesystem::path& filepath);

        void Clear(glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f));

        void SetSmooth(bool smooth);
        void SetRepeated(bool repeated);
    };

    class GpuTexture {
    private:
        unsigned int m_Handle{0u};
        unsigned int m_Binding{0u};

    public:
        ~GpuTexture();

        GpuTexture() = default;
        GpuTexture(unsigned int binding) : m_Binding(binding) {}

        GpuTexture(const GpuTexture&) = delete;
        GpuTexture& operator=(const GpuTexture&) = delete;

        GpuTexture(GpuTexture&& other) noexcept : m_Handle(other.m_Handle) {
            other.m_Handle = 0u;
        }

        GpuTexture& operator=(GpuTexture&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteTextures(1, &m_Handle);

                m_Handle = other.m_Handle;
                other.m_Handle = 0u;
            }

            return *this;
        }

        void Bind() const;

        void Create(unsigned int width, unsigned int height);

        void Clear(glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f));

        void SetSmooth(bool smooth);
        void SetRepeated(bool repeated);
    };
}