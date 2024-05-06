#pragma once

#include <glad/glad.h>

#include "IBindable.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class VBO;

        class VAO : public IBindable
        {
        private:
            GLuint _id;

        protected:
            virtual void OnBind() override;
            virtual void OnUnbind() override;

        public:
            VAO();
            ~VAO();

            GLuint GetId() const;

            void LinkAttribute(VBO& vbo, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizei stride, void* offset);
        };
    }
}