#include "Font.h"

#include "Texture.h"
#include "Vector2.h"
#include "Character.h"
#include "Vertex.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <utility>

using namespace OpenCL_PathTracer;
using namespace OpenCL_PathTracer::GL_Stuff;

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
    auto textureWidth = 0;
    for (unsigned char c = 32; c <= 126; c++)
    {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        maxAscent = std::max(maxAscent, face->glyph->bitmap_top);
        textureWidth = textureWidth + face->glyph->advance.x / 64;
    }
    auto textureHeight = ((face->size->metrics.ascender - face->size->metrics.descender) / 64);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    _texture = std::make_unique<class Texture>(GL_RED, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    _texture->Bind();
    auto pos = 0;
    for (unsigned char c = 32; c <= 126; c++)
    {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        auto size = Vector2(static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows));
        auto bearing = Vector2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top));
        auto yOffset = maxAscent - bearing.y;
        auto advance = face->glyph->advance.x / 64;
        _characters.insert({ c, Character(
            {
                { { bearing.x         ,          yOffset }, {  static_cast<float>(pos)           / textureWidth,                   0.0f } },
                { { bearing.x         , size.y + yOffset }, {  static_cast<float>(pos) / textureWidth, size.y / textureHeight } },
                { { bearing.x + size.x, size.y + yOffset }, { (static_cast<float>(pos) + size.x) / textureWidth, size.y / textureHeight } },
                { { bearing.x + size.x,          yOffset }, { (static_cast<float>(pos) + size.x) / textureWidth,                   0.0f } }
            },
            static_cast<float>(advance)
        ) });
        _texture->UpdateTexture(pos, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        pos = pos + advance;
    }
    _texture->Unbind();
    _texture->Freeze();

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

float Font::Height() const
{
    return _height;
}

std::shared_ptr<Texture> Font::Texture()
{
    return _texture;
}

Vector2 Font::MeasureTextSize(std::string const& text) const
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

Character const& Font::GetCharacter(char c) const
{
    return _characters.at(c);
}