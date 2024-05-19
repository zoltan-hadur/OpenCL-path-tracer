#include "StringHelper.h"

#include <stdexcept>
#include <format>
#include <bitset>

namespace OpenCL_PathTracer
{
    std::vector<uint32_t> StringHelper::GetUnicodeCharacters(std::string utf8text)
    {
        auto unicodeCharacters = std::vector<uint32_t>();
        int32_t unicodeCharacter = 0;
        auto remainingBytesCount = 0;
        for (uint8_t byte : utf8text)
        {
            if (remainingBytesCount > 0)
            {
                // 2nd/3rd/4th byte of utf8 encoded character
                if (byte >> 6 == 0b10)
                {
                    unicodeCharacter = (unicodeCharacter << 6) | (byte & 0b00111111);
                    remainingBytesCount--;
                }
                else
                {
                    throw std::runtime_error(std::format("Remaining bytes left: {0}. Byte '{1}' (0b{2}) should match mask 0b10xxxxxx as 2nd/3rd/4th byte of utf8 encoded character!", remainingBytesCount, byte, std::bitset<8>(byte).to_string()));
                }
            }
            // Single byte
            else if (byte >> 7 == 0b0)
            {
                unicodeCharacter = byte & 0b01111111;
                remainingBytesCount = 0;
            }
            // 1st byte of 2 bytes
            else if (byte >> 5 == 0b110)
            {
                unicodeCharacter = byte & 0b00011111;
                remainingBytesCount = 1;
            }
            // 1st byte of 3 bytes
            else if (byte >> 4 == 0b1110)
            {
                unicodeCharacter = byte & 0b00001111;
                remainingBytesCount = 2;
            }
            // 1st byte of 4 bytes
            else if (byte >> 3 == 0b11110)
            {
                unicodeCharacter = byte & 0b00000111;
                remainingBytesCount = 3;
            }
            else
            {
                throw std::runtime_error(std::format("Byte '{0}' (0b{1}) is neither a single byte ascii nor the 1st byte of 2/3/4 byte utf8 encoded character!", byte, std::bitset<8>(byte).to_string()));
            }

            if (remainingBytesCount == 0)
            {
                unicodeCharacters.push_back(unicodeCharacter);
            }
        }
        return unicodeCharacters;
    }

    std::string StringHelper::GetUtf8String(std::vector<uint32_t> unicodeCharacters)
    {
        auto utf8string = std::string();
        for (auto unicodeCharacter : unicodeCharacters)
        {
            // Single byte
            if (unicodeCharacter <= 0x007F)
            {
                utf8string.push_back(unicodeCharacter & 0b01111111);
            }
            // 2 bytes
            else if (unicodeCharacter <= 0x07FF)
            {
                utf8string.push_back(0b11000000 | ((unicodeCharacter & 0b00000000'00000111'11000000) >> 6));
                utf8string.push_back(0b10000000 | ((unicodeCharacter & 0b00000000'00000000'00111111) >> 0));
            }
            // 3 bytes
            else if (unicodeCharacter <= 0xFFFF)
            {
                utf8string.push_back(0b11100000 | ((unicodeCharacter & 0b00000000'11110000'00000000) >> 12));
                utf8string.push_back(0b10000000 | ((unicodeCharacter & 0b00000000'00001111'11000000) >> 6));
                utf8string.push_back(0b10000000 | ((unicodeCharacter & 0b00000000'00000000'00111111) >> 0));
            }
            // 4 bytes
            else if (unicodeCharacter <= 0x10FFFF)
            {
                utf8string.push_back(0b11110000 | ((unicodeCharacter & 0b00011100'00000000'00000000) >> 18));
                utf8string.push_back(0b10000000 | ((unicodeCharacter & 0b00000011'11110000'00000000) >> 12));
                utf8string.push_back(0b10000000 | ((unicodeCharacter & 0b00000000'00001111'11000000) >> 6));
                utf8string.push_back(0b10000000 | ((unicodeCharacter & 0b00000000'00000000'00111111) >> 0));
            }
        }
        return utf8string;
    }
}