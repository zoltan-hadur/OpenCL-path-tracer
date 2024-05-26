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
                // A line can be much longer than the console, so split it up into multiple lines
                for (int i = 0; i <= line.size() / _columnWidth; ++i)
                {
                    auto from = i * _columnWidth;
                    auto to = std::min((i + 1) * _columnWidth, static_cast<int>(line.size()));
                    // Handle when line is as long as the console
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
            // While the cursor is outside the console
            while (GetScreenCursor(_cursor).y + _charSize.y > _background->GetSize().y)
            {
                Scroll(ScrollDirection::Down, 1);
            }
        }

        void Console::UpdateScrollbarThumb()
        {
            auto textHeight = GetTextHeight();
            auto backgroundHeight = _background->GetSize().y;
            auto scrollbarThumbHeight = std::max(
                std::clamp(backgroundHeight / textHeight, 0.0f, 1.0f) * backgroundHeight,
                _minimumScrollbarThumbHeight
            );

            // Add epsilon so there will be no divide with 0
            auto visibleTextHeight = std::floor(backgroundHeight / _charSize.y) * _charSize.y + 0.0001f;
            auto currentPos = std::abs(_text->GetPosition().y);

            _scrollbarThumb->SetPosition(Vector2(
                _scrollbar->GetPosition().x,
                currentPos / (textHeight - visibleTextHeight) * (backgroundHeight - scrollbarThumbHeight)
            ));
            _scrollbarThumb->SetSize(Vector2(_scrollbarWidth, scrollbarThumbHeight));
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
            // Increase y for every subline in lines
            for (int i = 0; i < bufferCursor.y; ++i)
            {
                y = y + 1 + static_cast<int>(_buffer[i].size() - 1) / _columnWidth;
            }
            y = y + static_cast<int>(bufferCursor.x) / _columnWidth;
            float x = static_cast<int>(bufferCursor.x) % _columnWidth;
            return Vector2(x, y) * _charSize + _text->GetPosition();
        }

        float Console::GetTextHeight() const
        {
            return _font->MeasureTextSize(_text->GetValue()).y - _charSize.y;
        }

        Console::Console()
        {
            _background = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, 1), Color(0, 0.5f, 1, 0.68f));
            _font = std::make_shared<Font>("C:/Windows/Fonts/cour.ttf", 16);
            _text = std::make_unique<Text>(_font, "", Color(1, 1, 1));
            _caret = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, _font->GetHeight()), Color(1, 1, 1));
            _scrollbar = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, 1), Color(1.0f, 1.0f, 1.0f, 0.5f));
            _scrollbarThumb = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, 1), Color(0.5f, 0.5f, 0.5f, 0.5f));

            _buffer = { { {'>'} } };
            _cursor = { 1, 0 };
            _charSize = Vector2(_font->GetCharacter(' ').GetAdvance(), _font->GetHeight());
            _columnWidth = 1;
            _caretFrequency = 1.0f;
            _hasFocus = false;
            _isInsertTurnedOn = false;
            _minimumScrollbarThumbHeight = 50.0f;
            _scrollbarWidth = 20.0f;
            CopyBufferToText();

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
        }

        ConsoleState Console::GetState() const
        {
            return _state;
        }

        void Console::SetWindowSize(int width, int height)
        {
            _columnWidth = (width - _scrollbarWidth) / _charSize.x;
            CopyBufferToText();

            _positionAnimation.From = Vector2(0, -height);
            _positionAnimation.To = Vector2(0, 0);

            _background->SetSize(Vector2(width, height));
            _scrollbar->SetPosition(Vector2(width - _scrollbarWidth, 0));
            _scrollbar->SetSize(Vector2(width, height));

            if (GetScreenCursor(_cursor).y < std::floor(_background->GetSize().y / _charSize.y) * _charSize.y)
            {
                Scroll(ScrollDirection::Down, 1);
            }

            UpdateScrollbarThumb();
            UpdateCaretPosition();
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
            if (_isInsertTurnedOn && _cursor.x < line.size())
            {
                line[_cursor.x] = unicodeCodepoint;
            }
            else
            {
                line.insert(line.begin() + _cursor.x, unicodeCodepoint);
            }
            MoveCursor(CursorMovement::Right);
            CopyBufferToText();
            _caretAnimation.Start();
        }

        void Console::PressInsert()
        {
            _isInsertTurnedOn = !_isInsertTurnedOn;
            if (_isInsertTurnedOn)
            {
                _caret->SetSize(_charSize);
                auto color = _caret->GetColor();
                color.A = 0.5f;
                _caret->SetColor(color);
            }
            else
            {
                _caret->SetSize(Vector2(1, _charSize.y));
                auto color = _caret->GetColor();
                color.A = 1.0f;
                _caret->SetColor(color);
            }
            ScrollTextIfNeeded();
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
            ScrollTextIfNeeded();
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
            ScrollTextIfNeeded();
            _caretAnimation.Start();
        }

        void Console::PressEnter()
        {
            _buffer.push_back({ '>' });
            _cursor.x = 1;
            _cursor.y++;
            CopyBufferToText();
            ScrollTextIfNeeded();
            UpdateCaretPosition();
            _caretAnimation.Start();
        }

        void Console::MoveCursor(CursorMovement cursorMovement)
        {
            auto const& line = _buffer[_cursor.y];
            switch (cursorMovement)
            {
                case CursorMovement::Left:
                    _cursor.x = std::max(_cursor.x - 1.0f, 1.0f);
                    break;
                case CursorMovement::Right:
                    _cursor.x = std::min(_cursor.x + 1.0f, static_cast<float>(line.size()));
                    break;
                case CursorMovement::Home:
                    _cursor = { 1.0f, _buffer.size() - 1.0f };
                    break;
                case CursorMovement::End:
                    _cursor.x = line.size();
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

        void Console::Scroll(ScrollDirection direction, int times)
        {
            auto position = _text->GetPosition();
            // Allow additional scrolling when e.g. line is full, and next char would be on next line
            auto additional = (_cursor.y == _buffer.size() - 1 && static_cast<int>(_cursor.x) % _columnWidth == 0) ? _charSize.y : 0;
            auto newPosition = Vector2(
                position.x,
                std::clamp(
                    position.y + _charSize.y * times * ((direction == ScrollDirection::Up) ? 1 : -1),
                    std::min(std::floor(_background->GetSize().y / _charSize.y) * _charSize.y - GetTextHeight() - additional, 0.0f),
                    0.0f
                )
            );
            _text->SetPosition(newPosition);
            UpdateScrollbarThumb();
            UpdateCaretPosition();
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
            _scrollbar->Draw(shaderProgram);
            _scrollbarThumb->Draw(shaderProgram);
            shaderProgram.PopModelMatrix();
        }

        void Console::Animate(float dt)
        {
            _positionAnimation.Animate(dt);
            _caretAnimation.Animate(dt);
        }
    }
}