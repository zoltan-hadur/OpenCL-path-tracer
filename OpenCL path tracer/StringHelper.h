#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace OpenCL_PathTracer
{
    class StringHelper
    {
    public:
        static std::vector<uint32_t> GetUnicodeCharacters(std::string utf8text);
        static std::string GetUtf8String(std::vector<uint32_t> unicodeCharacters);
    };
}