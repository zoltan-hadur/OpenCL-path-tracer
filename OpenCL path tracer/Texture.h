#pragma once

#include <glad/glad.h>
#include "Bitmap.h"

class Texture
{
private:
    GLuint _id;

public:
    Texture(Bitmap const& bitmap);
    Texture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

    GLuint Id() const;
    void UpdateTexture(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) const;

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};