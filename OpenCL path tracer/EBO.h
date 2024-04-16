#pragma once

#include <glad/glad.h>
#include <vector>

class EBO
{
private:
    GLuint _id;

public:
    EBO(std::vector<GLuint> const& indices);

    GLuint Id() const;

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};