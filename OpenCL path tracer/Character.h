#pragma once

#include <vector>

class Vertex;

class Character
{
private:
    std::vector<Vertex> _vertices;
    float _advance;

public:
    Character(std::vector<Vertex> vertices, float advance);

    std::vector<Vertex> const& Vertices() const;
    float Advance() const;
};