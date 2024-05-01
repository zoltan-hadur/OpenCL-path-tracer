#pragma once

#include <memory>
#include <string>

#include "Color.h"

class Font;
class VAO;
class VBO;
class EBO;
class ShaderProgram;

class Text
{
private:
    std::shared_ptr<Font> _font;
    std::unique_ptr<VAO> _vao;
    std::unique_ptr<VBO> _vbo;
    std::unique_ptr<EBO> _ebo;
    size_t _indicesSize;
    std::string _value;
    Color _color;

public:
    Text(std::shared_ptr<Font> font, std::string text, Color color);

    std::string const& Value() const;
    void Value(std::string value);
    Color const& Color() const;
    void Color(class Color color);

    void Draw(ShaderProgram const& shaderProgram) const;

    void Bind() const;
    void Unbind() const;
};