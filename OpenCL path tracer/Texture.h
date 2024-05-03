#pragma once

#include <glad/glad.h>

#include "Freezable.h"

namespace OpenCL_PathTracer
{
    class Bitmap;

    namespace GL_Stuff
    {
        class Texture : public Freezable
        {
        private:
            GLuint _id;

        public:
            Texture(Bitmap const& bitmap);
            Texture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
            ~Texture();

            GLuint GetId() const;
            void UpdateTexture(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

            void Bind() const;
            void Unbind() const;
        };
    }
}