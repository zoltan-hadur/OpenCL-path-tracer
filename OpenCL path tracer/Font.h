#pragma once

#include <map>
#include <memory>
#include <filesystem>
#include <string>
#include <set>

namespace OpenCL_PathTracer
{
    class Vector2;

    namespace GL_Stuff
    {
        class Character;
        class Texture;

        class Font
        {
        private:
            float _height;
            std::map<uint32_t, Character> _characters;
            std::shared_ptr<Texture> _texture;
            static std::set<uint32_t> _supportedCharacters;

        public:
            Font(std::filesystem::path fontPath, uint8_t height);

            float GetHeight() const;
            std::shared_ptr<Texture> GetTexture();

            Vector2 MeasureTextSize(std::string const& text) const;
            Character const& GetCharacter(uint32_t c) const;
        };
    }
}