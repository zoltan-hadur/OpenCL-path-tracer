#include "Character.h"

#include "Vertex.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        Character::Character(std::vector<Vertex> vertices, float advance) : _vertices(std::move(vertices)), _advance(advance)
        {

        }

        std::vector<Vertex> const& Character::GetVertices() const
        {
            return _vertices;
        }

        float Character::GetAdvance() const
        {
            return _advance;
        }
    }
}