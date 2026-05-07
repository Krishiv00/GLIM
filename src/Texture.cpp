#include <cassert>
#include <vector>

#include "GLIM/Graphics/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace gl;

#pragma region Texture

Texture::~Texture() {
    if (m_Handle) glDeleteTextures(1, &m_Handle);
}

void Texture::Bind() const {
    assert(m_Handle && "texture has not been loaded yet");
    glBindTexture(GL_TEXTURE_2D, m_Handle);
}

bool Texture::LoadFromFile(const std::filesystem::path& filepath) {
    stbi_set_flip_vertically_on_load(true);

    int w, h, nc;

    uint8_t* data = stbi_load(filepath.string().c_str(), &w, &h, &nc, 0);

    if (!data) return false;

    if (m_Handle) glDeleteTextures(1, &m_Handle);
    glGenTextures(1, &m_Handle);

    Bind();

    SetRepeated(true);
    SetSmooth(false);

    unsigned int format;

    switch (nc) {
    case 1: format = GL_RED; break;
    case 2: format = GL_RG; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    default: stbi_image_free(data); return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return true;
}

void Texture::Clear(glm::vec4 color) {
    glClearTexImage(m_Handle, 0, GL_RGBA, GL_FLOAT, &color);
}

void Texture::SetSmooth(bool smooth) {
    Bind();

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        smooth ? GL_LINEAR : GL_NEAREST
    );
}

void Texture::SetRepeated(bool repeated) {
    Bind();

    const unsigned int wrapMode = repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
}

#pragma region Gpu Texture

GpuTexture::~GpuTexture() {
    if (m_Handle) glDeleteTextures(1, &m_Handle);
}

void GpuTexture::Bind() const {
    assert(m_Handle && "image has not been loaded yet");
    glBindTexture(GL_TEXTURE_2D, m_Handle);
    glBindImageTexture(m_Binding, m_Handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void GpuTexture::Create(unsigned int width, unsigned int height) {
    if (m_Handle) glDeleteTextures(1, &m_Handle);
    glGenTextures(1, &m_Handle);

    Bind();

    SetRepeated(true);
    SetSmooth(false);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);

    std::vector<glm::vec4> initialData(width * height, glm::vec4(0.f));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, initialData.data());

    Bind();
}

void GpuTexture::Clear(glm::vec4 color) {
    glClearTexImage(m_Handle, 0, GL_RGBA, GL_FLOAT, &color);
}

void GpuTexture::SetSmooth(bool smooth) {
    Bind();

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        smooth ? GL_LINEAR : GL_NEAREST
    );
}

void GpuTexture::SetRepeated(bool repeated) {
    Bind();

    const unsigned int wrapMode = repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
}