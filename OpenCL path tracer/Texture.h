#pragma once

#include <glad/glad.h>

#include "Freezable.h"
#include "IBindable.h"

namespace OpenCL_PathTracer
{
    class Bitmap;

    namespace GL_Stuff
    {
        class Texture : public Freezable, public IBindable
        {
        private:
            GLuint _id;

        protected:
            virtual void OnBind() override;
            virtual void OnUnbind() override;

        public:
            Texture(Bitmap const& bitmap);
            Texture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
            ~Texture();

            GLuint GetId() const;
            void UpdateTexture(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
        };
    }
}