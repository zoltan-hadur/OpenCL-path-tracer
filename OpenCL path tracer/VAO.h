#pragma once

#include <glad/glad.h>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class VBO;

        class VAO
        {
        private:
            GLuint _id;

        public:
            VAO();
            ~VAO();

            GLuint GetId() const;

            void LinkAttribute(VBO const& vbo, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizei stride, void* offset) const;
            void Bind() const;
            void Unbind() const;
        };
    }
}