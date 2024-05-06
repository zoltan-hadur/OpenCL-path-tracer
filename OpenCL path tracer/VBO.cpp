#include "VBO.h"

#include "Vertex.h"

#include <stdexcept>
#include <algorithm>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        void VBO::OnBind()
        {
            glBindBuffer(GL_ARRAY_BUFFER, _id);
        }

        void VBO::OnUnbind()
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        VBO::VBO(std::vector<Vertex> const& vertices)
        {
            glGenBuffers(1, &_id);
            ReplaceVertices(vertices);
        }

        VBO::~VBO()
        {
            glDeleteBuffers(1, &_id);
        }

        GLuint VBO::GetId() const
        {
            return _id;
        }

        BoundingBox const& VBO::GetBoundingBox() const
        {
            return _boundingBox;
        }

        void VBO::UpdateVertices(std::vector<Vertex> const& vertices)
        {
            if (vertices.size() != _size)
            {
                throw std::runtime_error("Size differs!");
            }
            _boundingBox = BoundingBox();
            std::ranges::for_each(vertices, [&](Vertex const& vertex) { _boundingBox.Inflate(vertex.GetPosition()); });
            Bind();
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * _size, vertices.data());
            Unbind();
        }

        void VBO::ReplaceVertices(std::vector<Vertex> const& vertices)
        {
            _size = vertices.size();
            _boundingBox = BoundingBox();
            std::ranges::for_each(vertices, [&](Vertex const& vertex) { _boundingBox.Inflate(vertex.GetPosition()); });
            Bind();
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
            Unbind();
        }
    }
}