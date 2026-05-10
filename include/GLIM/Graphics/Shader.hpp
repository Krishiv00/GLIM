#pragma once

#include <filesystem>

#include "GL/glew.h"

#include "GLIM/Graphics/TypeTraits.hpp"
#include "GLIM/Graphics/Camera.hpp"

namespace gl {
    class Shader final {
    private:
        unsigned int m_Handle{0u};

    public:
        Shader() = default;
        Shader(const std::filesystem::path& vertFilepath, const std::filesystem::path& fragFilepath);

        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&& other) noexcept : m_Handle(other.m_Handle) {
            other.m_Handle = 0u;
        }

        Shader& operator=(Shader&& other) noexcept {
            if (this != &other) {
                if (m_Handle) glDeleteProgram(m_Handle);

                m_Handle = other.m_Handle;
                other.m_Handle = 0u;
            }

            return *this;
        }

        template<typename T, typename... Args>
        void SetUniform(const std::string& name, Args... args) const {
            const GLint location = glGetUniformLocation(m_Handle, name.c_str());
            if (location == -1) return;

            UniformTraits<T, sizeof...(Args)>::Set(location, args...);
        }

        inline void SetUniform(const std::string& name, const Camera& camera) const {
            SetUniform<glm::mat4>(name, camera.GetVP());
        }

        [[nodiscard]]
        bool LoadFromFile(const std::filesystem::path& vertFilepath, const std::filesystem::path& fragFilepath);

        void Use() const;

        [[nodiscard]]
        inline unsigned int GetId() const noexcept {
            return m_Handle;
        }
    };
}