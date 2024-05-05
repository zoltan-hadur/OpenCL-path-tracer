#include "VAO.h"

#include "VBO.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        VAO::VAO()
        {
            glGenVertexArrays(1, &_id);
        }

        VAO::~VAO()
        {
            glDeleteVertexArrays(1, &_id);
        }

        GLuint VAO::GetId() const
        {
            return _id;
        }

        void VAO::LinkAttribute(VBO const& vbo, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizei stride, void* offset) const
        {
            glVertexAttribPointer(layout, numberOfComponents, type, GL_FALSE, stride, offset);
            glEnableVertexAttribArray(layout);
        }

        void VAO::Bind() const
        {
            glBindVertexArray(_id);
        }

        void VAO::Unbind() const
        {
            glBindVertexArray(0);
        }
    }
}