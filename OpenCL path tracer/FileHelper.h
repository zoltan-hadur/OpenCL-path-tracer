#pragma once

#include <string>
#include <filesystem>

namespace OpenCL_PathTracer
{
    class FileHelper
    {
    public:
        static std::string ReadAllText(std::filesystem::path path);
    };
}