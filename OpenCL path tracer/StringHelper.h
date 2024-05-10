#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace OpenCL_PathTracer
{
    class StringHelper
    {
    public:
        static std::vector<uint32_t> GetUnicodeCharacters(std::string text);
    };
}