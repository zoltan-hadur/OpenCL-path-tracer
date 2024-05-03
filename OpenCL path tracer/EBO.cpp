#include "EBO.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        EBO::EBO(std::vector<GLuint>  const& indices)
        {
            glGenBuffers(1, &_id);
            Bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
            Unbind();
        }

        EBO::~EBO()
        {
            glDeleteBuffers(1, &_id);
        }

        GLuint EBO::GetId() const
        {
            return _id;
        }

        std::size_t EBO::GetSize() const
        {
            return _size;
        }

        void EBO::UpdateIndices(std::vector<GLuint> const& indices)
        {
            if (indices.size() != _size)
            {
                throw std::runtime_error("Size differs!");
            }
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * _size, indices.data());
        }

        void EBO::ReplaceIndices(std::vector<GLuint> const& indices)
        {
            _size = indices.size();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
        }

        void EBO::Bind() const
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
        }

        void EBO::Unbind() const
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
}