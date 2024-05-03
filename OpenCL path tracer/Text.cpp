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
#include "Color.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        Text::Text(std::shared_ptr<Font> font, std::string value, Color color) : Component(ShaderMode::Text, Color(1, 1, 1), font->GetTexture())
        {
            _font = font;
            Bind();
            SetValue(value);
            Unbind();
        }

        std::string const& Text::GetValue() const
        {
            return _value;
        }

        void Text::SetValue(std::string value)
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
                        cursor.y = cursor.y + _font->GetHeight();
                        break;
                    }
                    case '\t':
                    {
                        // 1 tab = 2 spaces
                        cursor.x = cursor.x + _font->GetCharacter(' ').GetAdvance() * 2;
                        break;
                    }
                    default:
                    {
                        auto const& character = _font->GetCharacter(c);
                        for (auto const& vertex : character.GetVertices())
                        {
                            vertices.push_back({ vertex.GetPosition() + cursor, vertex.GetTextureCoordinate() });
                        }
                        for (auto index : defaultIndices)
                        {
                            indices.push_back(index + count * 4);
                        }
                        count++;
                        cursor.x = cursor.x + character.GetAdvance();
                        break;
                    }
                }
            }

            ReplaceData(vertices, indices);
        }
    }
}