#include "ShaderProgram.h"

#include "Shader.h"
#include "Color.h"
#include "ShaderMode.h"

#include <format>
#include <sstream>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        ShaderProgram::ShaderProgram(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath)
        {
            auto ss = std::stringstream();

            auto vertexShader = Shader(vertexShaderPath, GL_VERTEX_SHADER);
            if (!vertexShader.IsCompilationSuccessful())
            {
                ss << std::format("VertexShader compilation failed:\r\n{}", vertexShader.GetCompilationLog());
            }
            auto fragmentShader = Shader(fragmentShaderPath, GL_FRAGMENT_SHADER);
            if (!fragmentShader.IsCompilationSuccessful())
            {
                ss << std::format("FragmentShader compilation failed:\r\n{}", fragmentShader.GetCompilationLog());
            }

            _id = glCreateProgram();
            glAttachShader(_id, vertexShader.GetId());
            glAttachShader(_id, fragmentShader.GetId());
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

        GLuint ShaderProgram::GetId() const
        {
            return _id;
        }

        bool ShaderProgram::IsLinkingSuccessful() const
        {
            return _isLinkingSuccessful;
        }

        std::string const& ShaderProgram::GetLinkingLog() const
        {
            return _linkingLog;
        }

        void ShaderProgram::SetProjectionMatrix(Matrix4x4 const& projection)
        {
            glUniformMatrix4fv(_projectionMatrixLocation, 1, GL_TRUE, projection.GetData());
        }

        Matrix4x4 const& ShaderProgram::GetModelMatrix() const
        {
            return _modelMatrix;
        }

        void ShaderProgram::SetModelMatrix(Matrix4x4 const& model)
        {
            _modelMatrix = model;
            glUniformMatrix4fv(_modelMatrixLocation, 1, GL_TRUE, _modelMatrix.GetData());
        }

        void ShaderProgram::SetColor(Color const& color)
        {
            glUniform4f(_colorLocation, color.R, color.G, color.B, color.A);
        }

        void ShaderProgram::SetMode(ShaderMode mode)
        {
            glUniform1i(_modeLocation, static_cast<std::underlying_type<ShaderMode>::type>(mode));
        }

        void ShaderProgram::Activate() const
        {
            glUseProgram(_id);
            glUniform1i(_texture0Location, 0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}