#pragma once

#include <vector>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Vertex;

        class Character
        {
        private:
            std::vector<Vertex> _vertices;
            float _advance;

        public:
            Character(std::vector<Vertex> vertices, float advance);

            std::vector<Vertex> const& GetVertices() const;
            float GetAdvance() const;
        };
    }
}