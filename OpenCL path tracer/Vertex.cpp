#include "Vertex.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        Vertex::Vertex(Vector2 position) : Vertex(position, Vector2())
        {

        }

        Vertex::Vertex(Vector2 position, Vector2 textureCoordinate) : _position(position), _textureCoordinate(textureCoordinate)
        {

        }

        Vector2 const& Vertex::GetPosition() const
        {
            return _position;
        }

        Vector2 const& Vertex::GetTextureCoordinate() const
        {
            return _textureCoordinate;
        }
    }
}