#pragma once

#include <glad/glad.h>
#include <vector>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Vertex;

        class VBO
        {
        private:
            GLuint _id;
            std::size_t _size;

        public:
            VBO(std::vector<Vertex> const& vertices);
            ~VBO();

            GLuint Id() const;

            void UpdateVertices(std::vector<Vertex> const& vertices);
            void ReplaceVertices(std::vector<Vertex> const& vertices);

            void Bind() const;
            void Unbind() const;
        };
    }
}