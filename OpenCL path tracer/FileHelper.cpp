#include "FileHelper.h"

#include <fstream>
#include <sstream>

std::string FileHelper::ReadAllText(std::filesystem::path path)
{
    return (std::stringstream() << std::ifstream(path).rdbuf()).str();
}