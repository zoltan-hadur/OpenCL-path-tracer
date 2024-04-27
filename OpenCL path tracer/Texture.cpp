#include "Texture.h"

Texture::Texture(Bitmap const& bitmap) : Texture(GL_RGBA, bitmap.Width(), bitmap.Height(), GL_BGR, GL_UNSIGNED_BYTE, bitmap.Bytes().data())
{

}

Texture::Texture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
    glGenTextures(1, &_id);
    glActiveTexture(GL_TEXTURE0);
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, pixels);
    Unbind();
}

GLuint Texture::Id() const
{
    return _id;
}

void Texture::UpdateTexture(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) const
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, format, type, pixels);
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() const
{
    glDeleteTextures(1, &_id);
}
