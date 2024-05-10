#include "StringHelper.h"

#include <stdexcept>
#include <format>
#include <bitset>

namespace OpenCL_PathTracer
{
    std::vector<uint32_t> StringHelper::GetUnicodeCharacters(std::string text)
    {
        auto characters = std::vector<uint32_t>();
        int32_t character = 0;
        for (int i = 0; i < text.length(); ++i)
        {
            uint8_t c = text[i];
            if (c >> 7 == 0)
            {
                character = c;
            }
            else
            {
                auto count =
                    c >> 5 == 0b110 ? 2 :
                    c >> 4 == 0b1110 ? 3 :
                    c >> 3 == 0b11110 ? 4 :
                    throw std::runtime_error(std::format("Character '{0}' (0b{1}) is neither a single byte ascii nor the 1st byte of 2/3/4 byte character!", c, std::bitset<8>(c).to_string()));
                character = (0b1111'1111 >> count + 1) & c;
                for (int j = 1; j < count; ++j)
                {
                    c = text[++i];
                    character = (character << 6) | (c & 0b0011'1111);
                }
            }
            characters.push_back(character);
        }
        return characters;
    }
}