#include "Text.h"

#include "Font.h"
#include "Character.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "ShaderProgram.h"
#include "ShaderMode.h"
#include "Vector2.h"
#include "Vertex.h"

Text::Text(std::shared_ptr<Font> font, std::string text, class Color color)
{
    _font = font;
    _vao = std::make_unique<VAO>();
    _vbo = std::make_unique<VBO>(std::vector<Vertex>());
    _ebo = std::make_unique<EBO>(std::vector<GLuint>());
    Bind();

    Value(text);
    _color = color;

    _vao->LinkAttrib(*_vbo, 0, 4, GL_FLOAT, sizeof(float) * 4, (void*)0);
    _vao->LinkAttrib(*_vbo, 1, 4, GL_FLOAT, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    
    Unbind();
}

std::string const& Text::Value() const
{
    return _value;
}

void Text::Value(std::string value)
{
    _value = value;

    auto cursor = Vector2();
    auto vertices = std::vector<Vertex>();
    auto indices = std::vector<GLuint>();
    auto defaultIndices = std::vector<GLuint>
    {
        0, 1, 2,
        2, 3, 0
    };
    auto count = 0;
    for (auto c : _value)
    {
        switch (c)
        {
            case '\0':
            {
                continue;
            }
            case '\r':
            {
                cursor.x = 0;
                break;
            }
            case '\n':
            {
                cursor.y = cursor.y + _font->Height();
                break;
            }
            case '\t':
            {
                // 1 tab = 2 spaces
                cursor.x = cursor.x + _font->GetCharacter(' ').Advance() * 2;
                break;
            }
            default:
            {
                auto const& character = _font->GetCharacter(c);
                for (auto const& vertex : character.Vertices())
                {
                    vertices.push_back({ vertex.Position() + cursor, vertex.TextureCoordinate() });
                }
                for (auto index : defaultIndices)
                {
                    indices.push_back(index + count * 4);
                }
                count++;
                cursor.x = cursor.x + character.Advance();
                break;
            }
        }
    }

    _vbo->ReplaceVertices(vertices);
    _ebo->ReplaceIndices(indices);
    _indicesSize = indices.size();
}

Color const& Text::Color() const
{
    return _color;
}

void Text::Color(class Color color)
{
    _color = std::move(color);
}

void Text::Draw(ShaderProgram const& shaderProgram) const
{
    shaderProgram.Mode(ShaderMode::Text);
    shaderProgram.Color(_color);
    glDrawElements(GL_TRIANGLES, _indicesSize, GL_UNSIGNED_INT, 0);
}

void Text::Bind() const
{
    _font->Bind();
    _vao->Bind();
    _vbo->Bind();
    _ebo->Bind();
}

void Text::Unbind() const
{
    _font->Unbind();
    _vao->Unbind();
    _vbo->Unbind();
    _ebo->Unbind();
}
