#include "Font.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include "Texture.h"
#include "Vector2.h"
#include <utility>

Font::Font(std::filesystem::path fontPath, uint8_t height)
{
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, fontPath.string().c_str(), 0, &face);
    FT_Set_Char_Size(face, 0, height * 64, 0, 0);
    FT_Load_Char(face, ' ', FT_LOAD_RENDER);
    _height = face->size->metrics.height / 64.0f;

    // https://stackoverflow.com/questions/62374506/how-do-i-align-glyphs-along-the-baseline-with-freetype
    auto maxAscent = 0;
    for (unsigned char c = 32; c <= 126; c++)
    {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        maxAscent = std::max(maxAscent, face->glyph->bitmap_top);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 32; c <= 126; c++)
    {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        auto size = Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        auto bearing = Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        auto yOffset = maxAscent - bearing.y;
        _characters.insert({ c, Character(
            std::make_unique<Texture>(GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer),
            {
                { { bearing.x         ,          yOffset }, { 0.0f, 0.0f } },
                { { bearing.x         , size.y + yOffset }, { 0.0f, 1.0f } },
                { { bearing.x + size.x, size.y + yOffset }, { 1.0f, 1.0f } },
                { { bearing.x + size.x,          yOffset }, { 1.0f, 0.0f } }
            },
            face->glyph->advance.x / 64.0f
        ) });
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    auto vertices = std::vector<Vertex>
    {
        { { }, { } },
        { { }, { } },
        { { }, { } },
        { { }, { } }
    };
    auto indices = std::vector<GLuint>
    {
        0, 1, 2,
        2, 3, 0
    };

    _vao = std::make_unique<VAO>();
    _vao->Bind();
    _vbo = std::make_unique<VBO>(vertices);
    _vbo->Bind();
    _ebo = std::make_unique<EBO>(indices);
    _ebo->Bind();

    _vao->LinkAttrib(*_vbo, 0, 4, GL_FLOAT, sizeof(float) * 4, (void*)0);
    _vao->LinkAttrib(*_vbo, 1, 4, GL_FLOAT, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    _vao->Unbind();
    _vbo->Unbind();
    _ebo->Unbind();
}

float Font::Height() const
{
    return _height;
}

float Font::Draw(char c) const
{
    auto const& character = _characters.at(c);
    _vbo->Bind();
    _vbo->UpdateVertices(character.Vertices());
    _vbo->Unbind();
    character.Texture().Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return character.Advance();
}

Vector2 Font::MeasureText(std::string const& text) const
{
    auto size = Vector2();
    auto cursor = Vector2();
    if (text.length() > 0)
    {
        size.y = _height;
    }
    for (auto const& c : text)
    {
        auto const& character = _characters.at(c);
        switch (c)
        {
            case '\r':
            {
                cursor.x = 0;
                break;
            }
            case '\n':
            {
                cursor.y = cursor.y + _height;
                size.y = cursor.y + _height;
                break;
            }
            default:
            {
                cursor.x = cursor.x + character.Advance();
                size.x = std::max(size.x, cursor.x);
                break;
            }
        }
    }
    return size;
}

void Font::Bind() const
{
    _vao->Bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Font::Unbind() const
{
    _vao->Unbind();
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::Delete() const
{
    for (auto const& [letter, character] : _characters)
    {
        character.Delete();
    }
    _vao->Delete();
    _vbo->Delete();
    _ebo->Delete();
}