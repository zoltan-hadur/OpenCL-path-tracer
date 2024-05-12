#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <stack>

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

            std::stack<Matrix4x4> _modelMatrices;

            void SendTopModelMatrixToGpu();

        public:
            ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath);
            ~ShaderProgram();

            GLuint GetId() const;
            bool IsLinkingSuccessful() const;
            std::string const& GetLinkingLog() const;

            void SetProjectionMatrix(Matrix4x4 const& matrix);
            Matrix4x4 const& GetModelMatrix() const;
            void SetModelMatrix(Matrix4x4 const& matrix);
            void PushModelMatrix(Matrix4x4 const& matrix);
            void PopModelMatrix();
            void SetColor(Color const& color);

            void SetMode(ShaderMode mode);

            void Activate();
        };
    }
}