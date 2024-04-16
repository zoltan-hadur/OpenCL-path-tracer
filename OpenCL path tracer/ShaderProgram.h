#pragma once

#include <glad/glad.h>
#include <filesystem>
#include "Color.h"

class ShaderProgram
{
private:
    GLuint _id;
    GLint _colorLocation;

public:
    ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath);

    GLuint Id() const;
    void SetColor(Color const& color) const;

    void Activate() const;
    void Delete() const;
};