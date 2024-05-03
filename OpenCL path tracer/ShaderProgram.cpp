#include "ShaderProgram.h"

#include "Shader.h"
#include "Color.h"
#include "ShaderMode.h"

#include <format>
#include <sstream>

using namespace OpenCL_PathTracer;
using namespace OpenCL_PathTracer::GL_Stuff;

ShaderProgram::ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath)
{
    auto ss = std::stringstream();

    auto vertexShader = Shader(vertexShaderPath, GL_VERTEX_SHADER);
    if (!vertexShader.IsCompilationSuccessful())
    {
        ss << std::format("VertexShader compilation failed:\r\n{}", vertexShader.CompilationLog());
    }
    auto fragmentShader = Shader(fragmentShaderPath, GL_FRAGMENT_SHADER);
    if (!fragmentShader.IsCompilationSuccessful())
    {
        ss << std::format("FragmentShader compilation failed:\r\n{}", fragmentShader.CompilationLog());
    }

    _id = glCreateProgram();
    glAttachShader(_id, vertexShader.Id());
    glAttachShader(_id, fragmentShader.Id());
    glLinkProgram(_id);

    int isSuccess;
    glGetProgramiv(_id, GL_LINK_STATUS, &isSuccess);
    _isLinkingSuccessful = isSuccess == GL_TRUE;
    if (!_isLinkingSuccessful)
    {
        GLint logLength = 0;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);
        std::string log;
        log.resize(std::max(logLength - 1, 0));
        glGetShaderInfoLog(_id, logLength, NULL, log.data());
        ss << std::format("ShaderProgram linking failed:\r\n{}", log);
    }
    _linkingLog = ss.str();

    _projectionMatrixLocation = glGetUniformLocation(_id, "projectionMatrix");
    _modelMatrixLocation = glGetUniformLocation(_id, "modelMatrix");
    _colorLocation = glGetUniformLocation(_id, "color");
    _texture0Location = glGetUniformLocation(_id, "texture0");
    _modeLocation = glGetUniformLocation(_id, "mode");

    _modelMatrix = Matrix4x4::IdentityMatrix();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_id);
}

GLuint ShaderProgram::Id() const
{
    return _id;
}

bool ShaderProgram::IsLinkingSuccessful() const
{
    return _isLinkingSuccessful;
}

std::string const& ShaderProgram::LinkingLog() const
{
    return _linkingLog;
}

void ShaderProgram::ProjectionMatrix(Matrix4x4 const& projection)
{
    glUniformMatrix4fv(_projectionMatrixLocation, 1, GL_TRUE, projection.Data());
}

Matrix4x4 const& ShaderProgram::ModelMatrix() const
{
    return _modelMatrix;
}

void ShaderProgram::ModelMatrix(Matrix4x4 const& model)
{
    _modelMatrix = model;
    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_TRUE, _modelMatrix.Data());
}

void ShaderProgram::Color(class Color const& color)
{
    glUniform4f(_colorLocation, color.R, color.G, color.B, color.A);
}

void ShaderProgram::Mode(ShaderMode mode)
{
    glUniform1i(_modeLocation, static_cast<std::underlying_type<enum class Mode>::type>(mode));
}

void ShaderProgram::Activate() const
{
    glUseProgram(_id);
    glUniform1i(_texture0Location, 0);
}