#include "VBO.h"

VBO::VBO(std::vector<Vertex> const& vertices)
{
    glGenBuffers(1, &_id);
    Bind();
    ReplaceVertices(vertices);
    Unbind();
}

GLuint VBO::Id() const
{
    return _id;
}

void VBO::UpdateVertices(std::vector<Vertex> const& vertices) const
{
    if (vertices.size() != _size)
    {
        throw std::runtime_error("Size differs!");
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * _size, vertices.data());
}

void VBO::ReplaceVertices(std::vector<Vertex> const& vertices)
{
    _size = vertices.size();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
}

void VBO::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VBO::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() const
{
    glDeleteBuffers(1, &_id);
}
