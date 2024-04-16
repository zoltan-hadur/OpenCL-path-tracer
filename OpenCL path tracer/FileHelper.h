#pragma once

#include <string>
#include <filesystem>

class FileHelper
{
public:
    static std::string ReadAllText(std::filesystem::path path);
};