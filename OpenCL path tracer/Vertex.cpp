#include "Vertex.h"

Vertex::Vertex(Vector2 position) : Vertex(position, Vector2())
{

}

Vertex::Vertex(Vector2 position, Vector2 textureCoordinate) : _position(position), _textureCoordinate(textureCoordinate)
{

}

Vector2 const& Vertex::Position() const
{
    return _position;
}

Vector2 const& Vertex::TextureCoordinate() const
{
    return _textureCoordinate;
}
