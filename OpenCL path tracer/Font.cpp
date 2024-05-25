#include "Font.h"

#include "Texture.h"
#include "Vector2.h"
#include "Character.h"
#include "Vertex.h"
#include "StringHelper.h"
#include "BoundingBox.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <utility>
#include <stdexcept>
#include <format>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        std::set<uint32_t> Font::_supportedCharacters = []() -> auto
            {
                auto supportedCharacters = std::set<uint32_t>();
                for (uint32_t c = 32; c <= 126; ++c)
                {
                    supportedCharacters.insert(c);
                }
                for (uint32_t c = 160; c <= 767; ++c)
                {
                    supportedCharacters.insert(c);
                }
                supportedCharacters.insert(8364);   // Euro sign
                return supportedCharacters;
            }();

        Font::Font(std::filesystem::path fontPath, uint8_t height)
        {
            FT_Library ft;
            FT_Init_FreeType(&ft);
            FT_Face face;
            FT_New_Face(ft, fontPath.string().c_str(), 0, &face);
            FT_Set_Char_Size(face, 0, height * 64, 0, 0);
            FT_Load_Char(face, ' ', FT_LOAD_RENDER);
            _height = face->size->metrics.height / 64.0f;

            GLint maxTextureSize;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

            // https://stackoverflow.com/questions/62374506/how-do-i-align-glyphs-along-the-baseline-with-freetype
            // Calculate the texture size needed
            auto maxAscent = 0;
            auto cursor = Vector2(0, _height);
            auto textureBoundingBox = BoundingBox();
            textureBoundingBox.Inflate({ 0, 0 });
            textureBoundingBox.Inflate(cursor);
            for (auto c : _supportedCharacters)
            {
                FT_Load_Char(face, c, FT_LOAD_RENDER);
                maxAscent = std::max(maxAscent, face->glyph->bitmap_top);
                cursor.x = cursor.x + face->glyph->advance.x / 64;
                // If glyph would be outside of texture bounds, start a new line instead
                if (cursor.x > maxTextureSize)
                {
                    cursor.x = static_cast<float>(face->glyph->advance.x / 64);
                    cursor.y = cursor.y + _height;
                }
                textureBoundingBox.Inflate(cursor);
            }
            auto textureSize = textureBoundingBox.GetSize();
            if (textureSize.y > maxTextureSize)
            {
                throw std::runtime_error(std::format("Texture size {0}x{1} is bigger than maximum the supported texture size {2}x{2}!", textureSize.x, textureSize.y, maxTextureSize));
            }

            // Create OpenGL texture and fill it up with character glyphs
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            _texture = std::make_unique<Texture>(GL_RED, static_cast<GLsizei>(textureSize.x), static_cast<GLsizei>(textureSize.y), GL_RED, GL_UNSIGNED_BYTE, nullptr);
            _texture->Bind();
            cursor = Vector2();
            for (auto c : _supportedCharacters)
            {
                FT_Load_Char(face, c, FT_LOAD_RENDER);
                auto size = Vector2(static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows));
                auto bearing = Vector2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top));
                auto yOffset = maxAscent - bearing.y;
                auto advance = face->glyph->advance.x / 64;
                // If glyph would be outside of texture bounds, start a new line instead
                if (cursor.x + advance > textureBoundingBox.GetSize().x)
                {
                    cursor.x = 0;
                    cursor.y = cursor.y + _height;
                }
                _characters.insert({ c, Character(
                    {
                        { { bearing.x         ,          yOffset }, Vector2{  cursor.x          ,  cursor.y           } / textureSize },
                        { { bearing.x         , size.y + yOffset }, Vector2{  cursor.x          , (cursor.y + size.y) } / textureSize },
                        { { bearing.x + size.x, size.y + yOffset }, Vector2{ (cursor.x + size.x), (cursor.y + size.y) } / textureSize },
                        { { bearing.x + size.x,          yOffset }, Vector2{ (cursor.x + size.x),  cursor.y           } / textureSize }
                    },
                    static_cast<float>(advance)
                ) });
                _texture->UpdateTexture(
                    static_cast<GLint>(cursor.x), static_cast<GLint>(cursor.y),
                    static_cast<GLsizei>(face->glyph->bitmap.width), static_cast<GLsizei>(face->glyph->bitmap.rows),
                    GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
                );
                cursor.x = cursor.x + advance;
            }
            _texture->Unbind();
            _texture->Freeze();

            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }

        float Font::GetHeight() const
        {
            return _height;
        }

        std::shared_ptr<Texture> Font::GetTexture()
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
            for (auto const& c : StringHelper::GetUnicodeCharacters(text))
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
                        cursor.x = cursor.x + character.GetAdvance();
                        size.x = std::max(size.x, cursor.x);
                        break;
                    }
                }
            }
            return size;
        }

        Character const& Font::GetCharacter(uint32_t c) const
        {
            return _characters.at(c);
        }
    }
}