#include "Shader.h"

#include "FileHelper.h"

#include <iostream>

Shader::Shader(std::filesystem::path shaderPath, GLenum shaderType)
{
    auto shaderSource = FileHelper::ReadAllText(shaderPath);
    auto shaderSourceCstr = shaderSource.c_str();
    _id = glCreateShader(shaderType);
    glShaderSource(_id, 1, &shaderSourceCstr, nullptr);
    glCompileShader(_id);

    int isSuccess;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &isSuccess);
    if (isSuccess != GL_TRUE)
    {
        GLint logLength = 0;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);
        std::string log;
        log.resize(logLength);
        glGetShaderInfoLog(_id, logLength, NULL, log.data());
        std::cout << std::format("Shader compilation failed:\r\n{}", log) << std::endl;
    }
}

Shader::~Shader()
{
    glDeleteShader(_id);
}

GLuint Shader::Id() const
{
    return _id;
}
