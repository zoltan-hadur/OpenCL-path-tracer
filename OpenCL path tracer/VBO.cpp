#include "VBO.h"

#include "Vertex.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        VBO::VBO(std::vector<Vertex> const& vertices)
        {
            glGenBuffers(1, &_id);
            Bind();
            ReplaceVertices(vertices);
            Unbind();
        }

        VBO::~VBO()
        {
            glDeleteBuffers(1, &_id);
        }

        GLuint VBO::GetId() const
        {
            return _id;
        }

        void VBO::UpdateVertices(std::vector<Vertex> const& vertices)
        {
            if (vertices.size() != _size)
            {
                throw std::runtime_error("Size differs!");
            }
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * _size, vertices.data());
        }

        void VBO::ReplaceVertices(std::vector<Vertex> const& vertices)
        {
            _size = vertices.size();
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        }

        void VBO::Bind() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, _id);
        }

        void VBO::Unbind() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}