#pragma once

#include "Texture.h"
#include <vector>
#include "Vertex.h"
#include <memory>

class Character
{
private:
    std::unique_ptr<Texture> _texture;
    std::vector<Vertex> _vertices;
    float _advance;

public:
    Character(std::unique_ptr<Texture> texture, std::vector<Vertex> vertices, float advance);

    Texture const& Texture() const;
    std::vector<Vertex> const& Vertices() const;
    float Advance() const;

    void Delete() const;
};