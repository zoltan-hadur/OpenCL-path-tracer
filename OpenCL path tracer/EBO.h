#pragma once

#include <glad/glad.h>
#include <vector>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class EBO
        {
        private:
            GLuint _id;
            std::size_t _size;

        public:
            EBO(std::vector<GLuint> const& indices);
            ~EBO();

            GLuint GetId() const;
            std::size_t GetSize() const;

            void UpdateIndices(std::vector<GLuint> const& indices);
            void ReplaceIndices(std::vector<GLuint> const& indices);

            void Bind() const;
            void Unbind() const;
        };
    }
}