#include "Shader.h"

#include "FileHelper.h"

Shader::Shader(std::filesystem::path shaderPath, GLenum shaderType)
{
    auto shaderSource = FileHelper::ReadAllText(shaderPath);
    auto shaderSourceCstr = shaderSource.c_str();
    _id = glCreateShader(shaderType);
    glShaderSource(_id, 1, &shaderSourceCstr, nullptr);
    glCompileShader(_id);

    int isSuccess;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &isSuccess);
    _isCompilationSuccessful = isSuccess == GL_TRUE;
    if (!_isCompilationSuccessful)
    {
        GLint logLength = 0;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);
        _compilationLog.resize(std::max(logLength - 1, 0));
        glGetShaderInfoLog(_id, logLength, NULL, _compilationLog.data());
    }
    else
    {
        _compilationLog = "";
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

bool Shader::IsCompilationSuccessful() const
{
    return _isCompilationSuccessful;
}

std::string const& Shader::CompilationLog() const
{
    return _compilationLog;
}
