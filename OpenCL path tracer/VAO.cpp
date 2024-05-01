#include "VAO.h"

#include "VBO.h"

VAO::VAO()
{
    glGenVertexArrays(1, &_id);
}

GLuint VAO::Id() const
{
    return _id;
}

void VAO::LinkAttrib(VBO const& vbo, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizeiptr stride, void* offset) const
{
    vbo.Bind();
    glVertexAttribPointer(layout, numberOfComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo.Unbind();
}

void VAO::Bind() const
{
    glBindVertexArray(_id);
}

void VAO::Unbind() const
{
    glBindVertexArray(0);
}

void VAO::Delete() const
{
    glDeleteVertexArrays(1, &_id);
}
