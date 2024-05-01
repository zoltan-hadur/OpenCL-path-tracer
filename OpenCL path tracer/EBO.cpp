#include "EBO.h"

#include <stdexcept>

EBO::EBO(std::vector<GLuint>  const& indices)
{
    glGenBuffers(1, &_id);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
    Unbind();
}

GLuint EBO::Id() const
{
    return _id;
}

void EBO::UpdateIndices(std::vector<GLuint> const& indices) const
{
    if (indices.size() != _size)
    {
        throw std::runtime_error("Size differs!");
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * _size, indices.data());
}

void EBO::ReplaceIndices(std::vector<GLuint> const& indices)
{
    _size = indices.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void EBO::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void EBO::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() const
{
    glDeleteBuffers(1, &_id);
}
