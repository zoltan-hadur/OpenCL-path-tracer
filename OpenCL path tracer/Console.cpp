#include "Console.h"

#include "Vector3.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Font.h"
#include "Character.h"
#include "Vertex.h"
#include "StringHelper.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        void Console::CopyBufferToText()
        {
            auto text = std::string();
            for (auto const& line : _buffer)
            {
                for (int i = 0; i <= line.size() / _columnWidth; ++i)
                {
                    auto from = i * _columnWidth;
                    auto to = std::min((i + 1) * _columnWidth, static_cast<int>(line.size()));
                    if (from == to)
                    {
                        break;
                    }
                    auto subLine = std::vector<uint32_t>(line.begin() + from, line.begin() + to);
                    text = text + StringHelper::GetUtf8String(subLine) + "\r\n";
                }
            }
            _text->SetValue(text);
        }

        void Console::UpdateCaretPosition()
        {
            _caret->SetPosition(GetScreenCursor(_cursor));
        }

        void Console::ScrollTextIfNeeded()
        {
            if (GetScreenCursor(_cursor).y + _charSize.y > _background->GetSize().y)
            {
                _text->SetPosition(_text->GetPosition() - Vector2(0.0f, _charSize.y));
            }
        }

        Vector2 Console::GetBufferCursor(Vector2 const& screenCursor) const
        {
            float y = 0;
            for (auto const& line : _buffer)
            {
                auto lineCount = line.size() / (_columnWidth + 1);
                if ((y + lineCount) < screenCursor.y)
                {
                    y = y + lineCount;
                }
                else
                {
                    auto x = (screenCursor.y - y) * _columnWidth + screenCursor.x;
                    return { x, y };
                }
            }
            return { -1, -1 };
        }

        Vector2 Console::GetScreenCursor(Vector2 const& bufferCursor) const
        {
            float y = 0;
            for (int i = 0; i < bufferCursor.y; ++i)
            {
                y = y + 1 + _buffer[i].size() / (_columnWidth + 1);
            }
            y = y + static_cast<int>(bufferCursor.x) / _columnWidth;
            float x = static_cast<int>(bufferCursor.x) % _columnWidth;
            return Vector2(x, y) * _charSize + _text->GetPosition();
        }

        Console::Console()
        {
            _background = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, 1), Color(0, 0.5f, 1, 0.68f));
            _font = std::make_shared<Font>("C:/Windows/Fonts/cour.ttf", 16);
            _text = std::make_unique<Text>(_font, "", Color(1, 1, 1));
            _caret = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, _font->GetHeight()), Color(1, 1, 1));

            _buffer = { { {'>'} } };
            _cursor = { 1, 0 };
            _charSize = Vector2(_font->GetCharacter(' ').GetAdvance(), _font->GetHeight());
            _columnWidth = 1;
            _caretFrequency = 1.0f;
            _hasFocus = false;

            _state = ConsoleState::Closed;
            _positionAnimation = Animation<Vector2>();
            _positionAnimation.SetOnFinishedCallback([&](Animation<Vector2>& animation)
                {
                    animation.Reverse();
                    switch (_state)
                    {
                        case ConsoleState::Opening:
                            _state = ConsoleState::Open;
                            break;
                        case ConsoleState::Closing:
                            _state = ConsoleState::Closed;
                            break;
                    }
                });
            _caretAnimation = Animation<float>(0.0f, 1.0f, 1.0f / _caretFrequency, EasingFunction(EasingMode::EaseIn, [](float t) { return t; }));
            _caretAnimation.SetOnFinishedCallback([](Animation<float>& animation)
                {
                    animation.Start();
                });

            UpdateCaretPosition();
            CopyBufferToText();
        }

        ConsoleState Console::GetState() const
        {
            return _state;
        }

        void Console::SetWindowSize(int width, int height)
        {
            _positionAnimation.From = Vector2(0, -height);
            _positionAnimation.To = Vector2(0, 0);

            _background->SetSize(Vector2(width, height));

            _columnWidth = width / _charSize.x;

            UpdateCaretPosition();
            CopyBufferToText();
        }

        void Console::Open()
        {
            _state = ConsoleState::Opening;
            _positionAnimation.Start();
            _caretAnimation.Start();
        }

        void Console::Close()
        {
            _state = ConsoleState::Closing;
            _positionAnimation.Start();
        }

        void Console::TypeCharacter(uint32_t unicodeCodepoint)
        {
            auto& line = _buffer[_cursor.y];
            line.insert(line.begin() + _cursor.x, unicodeCodepoint);
            MoveCursor(CursorMovement::Right);
            CopyBufferToText();
            _caretAnimation.Start();
        }

        void Console::PressBackspace()
        {
            if (_cursor.x > 1)
            {
                auto& line = _buffer[_cursor.y];
                line.erase(line.begin() + _cursor.x - 1);
                MoveCursor(CursorMovement::Left);
                CopyBufferToText();
            }
            _caretAnimation.Start();
        }

        void Console::PressDelete()
        {
            auto& line = _buffer[_cursor.y];
            if (_cursor.x < line.size())
            {
                line.erase(line.begin() + _cursor.x);
                CopyBufferToText();
            }
            _caretAnimation.Start();
        }

        void Console::PressEnter()
        {
            _buffer.push_back({ '>' });
            _cursor.x = 1;
            _cursor.y++;
            ScrollTextIfNeeded();
            UpdateCaretPosition();
            CopyBufferToText();
            _caretAnimation.Start();
        }

        void Console::MoveCursor(CursorMovement cursorMovement)
        {
            switch (cursorMovement)
            {
                case CursorMovement::Left:
                    _cursor.x = std::max(_cursor.x - 1.0f, 1.0f);
                    break;
                case CursorMovement::Right:
                    _cursor.x = std::min(_cursor.x + 1.0f, static_cast<float>(_buffer[_cursor.y].size()));
                    break;
                case CursorMovement::Home:
                    _cursor = { 1.0f, _buffer.size() - 1.0f };
                    break;
                case CursorMovement::End:
                    _cursor.x = _buffer[_cursor.y].size();
                    break;
                default:
                    break;
            }
            ScrollTextIfNeeded();
            UpdateCaretPosition();
            _caretAnimation.Start();
        }

        void Console::Focus()
        {
            _hasFocus = true;
            _caretAnimation.Start();
        }

        void Console::ClearFocus()
        {
            _hasFocus = false;
        }

        void Console::Draw(ShaderProgram& shaderProgram) const
        {
            if (_state == ConsoleState::Closed) return;
            shaderProgram.PushModelMatrix(Matrix4x4::TranslateMatrix({ _positionAnimation.GetCurrentValue(), 0.0f }));
            _background->Draw(shaderProgram);
            _text->Draw(shaderProgram);
            if (_hasFocus)
            {
                if (_caretAnimation.GetCurrentValue() < 0.5f)
                {
                    _caret->Draw(shaderProgram);
                }
            }
            shaderProgram.PopModelMatrix();
        }

        void Console::Animate(float dt)
        {
            _positionAnimation.Animate(dt);
            _caretAnimation.Animate(dt);
        }
    }
}