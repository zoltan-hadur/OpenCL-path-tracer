#include "VBO.h"

VBO::VBO(std::vector<Vertex> const& vertices)
{
    glGenBuffers(1, &_id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    Unbind();
}

GLuint VBO::Id() const
{
    return _id;
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
