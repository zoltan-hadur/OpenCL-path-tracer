#pragma once

#include <map>
#include <memory>
#include <filesystem>
#include <string>

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
            std::map<char, Character> _characters;
            std::shared_ptr<Texture> _texture;

        public:
            Font(std::filesystem::path fontPath, uint8_t height);

            float Height() const;
            std::shared_ptr<Texture> Texture();

            Vector2 MeasureTextSize(std::string const& text) const;
            Character const& GetCharacter(char c) const;
        };
    }
}