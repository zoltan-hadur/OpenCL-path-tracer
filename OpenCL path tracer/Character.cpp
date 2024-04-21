#include "Character.h"

Character::Character(std::unique_ptr<class Texture> texture, std::vector<Vertex> vertices, float advance) :
    _texture(std::move(texture)), _vertices(std::move(vertices)), _advance(advance)
{

}

Texture const& Character::Texture() const
{
    return *_texture;
}

std::vector<Vertex> const& Character::Vertices() const
{
    return _vertices;
}

float Character::Advance() const
{
    return _advance;
}

void Character::Delete() const
{
    _texture->Delete();
}
