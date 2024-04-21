#pragma once

#include <filesystem>
#include <map>
#include "Character.h"
#include <memory>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class Font
{
private:
    float _height;
    std::map<char, Character> _characters;
    std::unique_ptr<VAO> _vao;
    std::unique_ptr<VBO> _vbo;
    std::unique_ptr<EBO> _ebo;

public:
    Font(std::filesystem::path fontPath, uint8_t height);

    void Draw(char c) const;

    void Bind() const;
    void Unbind() const;
    void Delete() const;
};