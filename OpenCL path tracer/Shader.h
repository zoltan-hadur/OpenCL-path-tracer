#pragma once

#include <glad/glad.h>
#include <filesystem>

class Shader
{
private:
    GLuint _id;

public:
    Shader(std::filesystem::path shaderPath, GLenum shaderType);

    GLuint Id() const;
    void Delete() const;
};