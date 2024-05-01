#pragma once

#include <glad/glad.h>
#include <vector>

class Vertex;

class VBO
{
private:
    GLuint _id;
    std::size_t _size;

public:
    VBO(std::vector<Vertex> const& vertices);
    ~VBO();

    GLuint Id() const;

    void UpdateVertices(std::vector<Vertex> const& vertices) const;
    void ReplaceVertices(std::vector<Vertex> const& vertices);

    void Bind() const;
    void Unbind() const;
};