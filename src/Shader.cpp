#include <fstream>
#include <stdexcept>

#ifndef NDEBUG
#include <iostream>
#endif

#include "GLIM/Graphics/Shader.hpp"

using namespace gl;

static std::string ReadFile(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

#ifndef NDEBUG

static bool CheckShaderCompile(GLuint shader, const std::string& name) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

        std::cerr << "[SHADER ERROR] (" << name << "):" << std::endl << infoLog << std::endl;

        return false;
    }

    return true;
}

static bool CheckProgramLink(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);

        std::cerr << "[PROGRAM LINK ERROR]:" << std::endl << infoLog << std::endl;

        return false;
    }

    return true;
}

#endif

Shader::Shader(const std::filesystem::path& vertFilepath, const std::filesystem::path& fragFilepath) {
    (void)LoadFromFile(vertFilepath, fragFilepath);
}

Shader::~Shader() {
    if (m_Handle) glDeleteProgram(m_Handle);
}

bool Shader::LoadFromFile(const std::filesystem::path& vertFilepath, const std::filesystem::path& fragFilepath) {
    if (m_Handle) glDeleteProgram(m_Handle);

    // create objects
    m_Handle = glCreateProgram();

    Use();

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // compile shaders
    const std::string vertSrc = ReadFile(vertFilepath);
    const char* vertexShaderSource = vertSrc.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

#ifndef NDEBUG
    if (!CheckShaderCompile(vertexShader, "VERTEX")) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        return false;
    }
#endif

    const std::string fragSrc = ReadFile(fragFilepath);
    const char* fragmentShaderSource = fragSrc.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

#ifndef NDEBUG
    if (!CheckShaderCompile(fragmentShader, "FRAGMENT")) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        return false;
    }
#endif

    // link shaders
    glAttachShader(m_Handle, vertexShader);
    glAttachShader(m_Handle, fragmentShader);
    glLinkProgram(m_Handle);

#ifndef NDEBUG
    if (!CheckProgramLink(m_Handle)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_Handle);
        m_Handle = 0u;

        return false;
    }
#endif

    // cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Use();

    return true;
}

void Shader::Use() const {
    glUseProgram(m_Handle);
}