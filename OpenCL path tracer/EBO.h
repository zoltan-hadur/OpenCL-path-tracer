#pragma once

#include <glad/glad.h>
#include <vector>

class EBO
{
private:
    GLuint _id;
    std::size_t _size;

public:
    EBO(std::vector<GLuint> const& indices);
    ~EBO();

    GLuint Id() const;

    void UpdateIndices(std::vector<GLuint> const& indices) const;
    void ReplaceIndices(std::vector<GLuint> const& indices);

    void Bind() const;
    void Unbind() const;
};