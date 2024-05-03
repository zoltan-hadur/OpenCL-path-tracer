#pragma once

#include <glad/glad.h>
#include <filesystem>

#include "Matrix4x4.h"

namespace OpenCL_PathTracer
{
    class Color;

    namespace GL_Stuff
    {
        enum class ShaderMode;

        class ShaderProgram
        {
        private:
            GLuint _id;
            bool _isLinkingSuccessful;
            std::string _linkingLog;

            GLint _projectionMatrixLocation;
            GLint _modelMatrixLocation;
            GLint _colorLocation;
            GLint _texture0Location;
            GLint _modeLocation;

            Matrix4x4 _modelMatrix;

        public:
            ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath);
            ~ShaderProgram();

            GLuint Id() const;
            bool IsLinkingSuccessful() const;
            std::string const& LinkingLog() const;

            void ProjectionMatrix(Matrix4x4 const& matrix) const;
            Matrix4x4 const& ModelMatrix() const;
            void ModelMatrix(Matrix4x4 const& matrix);
            void Color(Color const& color) const;

            void Mode(ShaderMode mode) const;

            void Activate() const;
        };
    }
}