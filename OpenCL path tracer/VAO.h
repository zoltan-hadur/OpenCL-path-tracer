#pragma once

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
private:
    GLuint _id;

public:
    VAO();

    GLuint Id() const;

    void LinkAttrib(VBO const& vbo, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizeiptr stride, void* offset) const;
    void Bind() const;
    void Unbind() const;
    void Delete() const;
};