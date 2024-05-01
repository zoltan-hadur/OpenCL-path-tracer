#pragma once

#include <map>
#include <memory>
#include <filesystem>
#include <string>

class Character;
class Texture;
class Vector2;

class Font
{
private:
    float _height;
    std::map<char, Character> _characters;
    std::unique_ptr<Texture> _texture;

public:
    Font(std::filesystem::path fontPath, uint8_t height);

    float Height() const;

    Vector2 MeasureTextSize(std::string const& text) const;
    Character const& GetCharacter(char c) const;

    void Bind() const;
    void Unbind() const;
};