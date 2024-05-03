#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <string>

class Shader
{
private:
    GLuint _id;
    bool _isCompilationSuccessful;
    std::string _compilationLog;

public:
    Shader(std::filesystem::path shaderPath, GLenum shaderType);
    ~Shader();

    GLuint Id() const;
    bool IsCompilationSuccessful() const;
    std::string const& CompilationLog() const;
};