#include "Texture.h"

#include "Bitmap.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        void Texture::OnBind()
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, _id);
        }

        void Texture::OnUnbind()
        {
            glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Texture::Texture(Bitmap const& bitmap) : Texture(GL_RGBA, bitmap.GetWidth(), bitmap.GetHeight(), GL_BGR, GL_UNSIGNED_BYTE, bitmap.GetData().data())
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

        Texture::~Texture()
        {
            glDeleteTextures(1, &_id);
        }

        GLuint Texture::GetId() const
        {
            return _id;
        }

        void Texture::UpdateTexture(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
        {
            ThrowIfFrozen();
            Bind();
            glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, format, type, pixels);
            Unbind();
        }
    }
}