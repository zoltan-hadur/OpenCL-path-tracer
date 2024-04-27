#include "Character.h"

Character::Character(std::vector<Vertex> vertices, float advance) : _vertices(std::move(vertices)), _advance(advance)
{

}

std::vector<Vertex> const& Character::Vertices() const
{
    return _vertices;
}

float Character::Advance() const
{
    return _advance;
}
