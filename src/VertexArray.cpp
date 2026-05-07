#include "GLIM/Graphics/VertexArray.hpp"

using namespace gl;

VertexArray::VertexArray(PrimitiveType primitiveType) {
    SetPrimitiveType(primitiveType);
}

VertexArray::~VertexArray() {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void VertexArray::SetPrimitiveType(PrimitiveType type) {
    m_PrimitiveType = static_cast<unsigned int>(type);
}

void VertexArray::Clear() {
    m_Data.clear();
    m_Dirty = true;
}

void VertexArray::Upload() const {
    assert(m_Data.size() % m_Stride == 0 && "data must match vertex layout");

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_Data.size(), m_Data.data(), GL_STATIC_DRAW);

    for (unsigned int i = 0; i < m_Layout.size(); ++i) {
        const Element e = m_Layout[i];

        glEnableVertexAttribArray(i);

        if (e.Type == GL_FLOAT) {
            glVertexAttribPointer(
                i, e.Count, e.Type, e.Normalized, m_Stride, (const void*)(uintptr_t)e.Offset
            );
        } else {
            glVertexAttribIPointer(
                i, e.Count, e.Type, m_Stride, (const void*)(uintptr_t)e.Offset
            );
        }
    }

    m_Dirty = false;
}

void VertexArray::Draw() const {
    if (m_Dirty) Upload();

    glBindVertexArray(m_VAO);
    glDrawArrays(m_PrimitiveType, 0, GetVertexCount());
}

unsigned int VertexArray::GetVertexCount() const {
    return m_Stride == 0 ? 0 : m_Data.size() / m_Stride;
}