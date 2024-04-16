#include "Vertex.h"

Vertex::Vertex(Vector2 position) : Vertex(position, Vector2())
{

}

Vertex::Vertex(Vector2 position, Vector2 textureCoordinate) : _position(position), _textureCoordinate(textureCoordinate)
{

}