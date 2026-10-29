#pragma once

#include <filesystem>

#include "GL/glew.h"

#include "glm/glm.hpp"

namespace gl {
    class Texture {
    private:
        unsigned int m_Handle{0u};
        unsigned int m_Binding{0u};

    public:
        ~Texture();

        Texture() = default;
        Texture(unsigned int binding) : m_Binding(binding) {}

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept : m_Handle(other.m_Handle), m_Binding(other.m_Binding) {
            other.m_Handle = 0u;
        }

        Texture& operator=(Texture&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteTextures(1, &m_Handle);

                m_Handle = other.m_Handle;
                m_Binding = other.m_Binding;

                other.m_Handle = 0u;
            }

            return *this;
        }

        void SetBinding(unsigned int binding);

        void Bind() const;

        [[nodiscard]]
        bool LoadFromFile(const std::filesystem::path& filepath);

        void Clear(glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f));

        void SetSmooth(bool smooth);
        void SetRepeated(bool repeated);

        [[nodiscard]]
        inline unsigned int GetId() const noexcept {
            return m_Handle;
        }

        [[nodiscard]]
        inline unsigned int GetBinding() const noexcept {
            return m_Binding;
        }
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

        GpuTexture(GpuTexture&& other) noexcept : m_Handle(other.m_Handle), m_Binding(other.m_Binding) {
            other.m_Handle = 0u;
        }

        GpuTexture& operator=(GpuTexture&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteTextures(1, &m_Handle);

                m_Handle = other.m_Handle;
                m_Binding = other.m_Binding;

                other.m_Handle = 0u;
            }

            return *this;
        }

        void SetBinding(unsigned int binding);

        void Bind() const;

        void Create(unsigned int width, unsigned int height);

        void Clear(glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f));

        void SetSmooth(bool smooth);
        void SetRepeated(bool repeated);

        [[nodiscard]]
        inline unsigned int GetId() const noexcept {
            return m_Handle;
        }

        [[nodiscard]]
        inline unsigned int GetBinding() const noexcept {
            return m_Binding;
        }
    };

    class CubemapTexture {
    private:
        unsigned int m_Handle{0u};
        unsigned int m_Binding{0u};

    public:
        ~CubemapTexture();

        CubemapTexture() = default;
        CubemapTexture(unsigned int binding) : m_Binding(binding) {}

        CubemapTexture(const CubemapTexture&) = delete;
        CubemapTexture& operator=(const CubemapTexture&) = delete;

        CubemapTexture(CubemapTexture&& other) noexcept : m_Handle(other.m_Handle), m_Binding(other.m_Binding) {
            other.m_Handle = 0u;
        }

        CubemapTexture& operator=(CubemapTexture&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteTextures(1, &m_Handle);
                m_Handle = other.m_Handle;
                m_Binding = other.m_Binding;
                other.m_Handle = 0u;
            }

            return *this;
        }

        void SetBinding(unsigned int binding);
        void Bind() const;

        [[nodiscard]]
        bool LoadFromFile(const std::filesystem::path& filepath);

        void SetSmooth(bool smooth);

        [[nodiscard]]
        inline unsigned int GetId() const noexcept {
            return m_Handle;
        }

        [[nodiscard]]
        inline unsigned int GetBinding() const noexcept {
            return m_Binding;
        }
    };
}
