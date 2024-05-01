#pragma once

#include <filesystem>
#include <map>
#include "Character.h"
#include <memory>
#include "Texture.h"

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