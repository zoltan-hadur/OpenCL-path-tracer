#include "ShaderProgram.h"
#include "Shader.h"
#include <iostream>

ShaderProgram::ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath)
{
    auto vertexShader = Shader(vertexShaderPath, GL_VERTEX_SHADER);
    auto fragmentShader = Shader(fragmentShaderPath, GL_FRAGMENT_SHADER);

    _id = glCreateProgram();
    glAttachShader(_id, vertexShader.Id());
    glAttachShader(_id, fragmentShader.Id());
    glLinkProgram(_id);

    int isSuccess;
    glGetProgramiv(_id, GL_LINK_STATUS, &isSuccess);
    if (isSuccess != GL_TRUE)
    {
        GLint logLength = 0;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);
        std::string log;
        log.resize(logLength);
        glGetShaderInfoLog(_id, logLength, NULL, log.data());
        std::cout << std::format("Shader linking failed:\r\n{}", log) << std::endl;
    }

    vertexShader.Delete();
    fragmentShader.Delete();

    _colorLocation = glGetUniformLocation(_id, "color");
}

GLuint ShaderProgram::Id() const
{
    return _id;
}

void ShaderProgram::SetColor(Color const& color) const
{
    glUniform4f(_colorLocation, color.R, color.G, color.B, color.A);
}

void ShaderProgram::Activate() const
{
    glUseProgram(_id);
}

void ShaderProgram::Delete() const
{
    glDeleteProgram(_id);
}