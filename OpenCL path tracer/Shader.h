#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <string>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        class Shader
        {
        private:
            GLuint _id;
            bool _isCompilationSuccessful;
            std::string _compilationLog;

        public:
            Shader(std::filesystem::path shaderPath, GLenum shaderType);
            ~Shader();

            GLuint GetId() const;
            bool IsCompilationSuccessful() const;
            std::string const& GetCompilationLog() const;
        };
    }
}