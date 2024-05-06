#include "VAO.h"

#include "VBO.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        void VAO::OnBind()
        {
            glBindVertexArray(_id);
        }

        void VAO::OnUnbind()
        {
            glBindVertexArray(0);
        }

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

        void VAO::LinkAttribute(VBO& vbo, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizei stride, void* offset)
        {
            Bind();
            vbo.Bind();
            glVertexAttribPointer(layout, numberOfComponents, type, GL_FALSE, stride, offset);
            glEnableVertexAttribArray(layout);
            Unbind();
            vbo.Unbind();
        }
    }
}