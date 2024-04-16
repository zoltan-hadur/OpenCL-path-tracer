#pragma once

#include "Vector2.h"

class Vertex
{
private:
    Vector2 _position;
    Vector2 _textureCoordinate;

public:
    Vertex(Vector2 position);
    Vertex(Vector2 position, Vector2 textureCoordinate);
};