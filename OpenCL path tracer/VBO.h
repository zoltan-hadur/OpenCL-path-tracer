#pragma once

#include <glad/glad.h>
#include <vector>

#include "Vertex.h"

class VBO
{
private:
    GLuint _id;

public:
    VBO(std::vector<Vertex> const& vertices);

    GLuint Id() const;

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};