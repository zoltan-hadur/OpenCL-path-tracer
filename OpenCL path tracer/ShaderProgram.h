#pragma once

#include <glad/glad.h>
#include <filesystem>
#include "Color.h"
#include "Matrix4x4.h"

enum class Mode
{
    Color = 0,
    Texture = 1,
    Text = 2
};

class ShaderProgram
{
private:
    GLuint _id;

    GLint _projectionMatrixLocation;
    GLint _modelMatrixLocation;
    GLint _colorLocation;
    GLint _texture0Location;
    GLint _modeLocation;

    Matrix4x4 _modelMatrix;

public:
    ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath);

    GLuint Id() const;

    void ProjectionMatrix(Matrix4x4 const& matrix) const;
    Matrix4x4 const& ModelMatrix() const;
    void ModelMatrix(Matrix4x4 const& matrix);
    void Color(Color const& color) const;

    void Mode(Mode mode) const;

    void Activate() const;
    void Delete() const;
};