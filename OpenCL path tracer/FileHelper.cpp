#include "FileHelper.h"

#include <fstream>
#include <sstream>

namespace OpenCL_PathTracer
{
    std::string FileHelper::ReadAllText(std::filesystem::path path)
    {
        return (std::stringstream() << std::ifstream(path, std::ios::binary).rdbuf()).str();
    }
}