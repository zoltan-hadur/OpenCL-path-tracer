#pragma once

#include <memory>
#include <vector>

#include "Panel.h"
#include "Text.h"
#include "Animation.h"
#include "Vector2.h"
#include "ShaderProgram.h"

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        enum class ConsoleState
        {
            Closed,
            Opening,
            Open,
            Closing
        };

        enum class CursorMovement
        {
            Left,
            Right,
            Home,
            End
        };

        class Console
        {
        private:
            std::unique_ptr<Panel> _background;
            std::shared_ptr<Font> _font;
            std::unique_ptr<Text> _text;
            std::unique_ptr<Panel> _caret;

            std::vector<std::vector<uint32_t>> _buffer;
            Vector2 _cursor;
            Vector2 _charSize;
            int _columnWidth;
            float _caretFrequency;
            bool _hasFocus;
            bool _isInsertTurnedOn;

            ConsoleState _state;
            Animation<Vector2> _positionAnimation;
            Animation<float> _caretAnimation;

            void CopyBufferToText();
            void UpdateCaretPosition();
            void ScrollTextIfNeeded();

            Vector2 GetBufferCursor(Vector2 const& screenCursor) const;
            Vector2 GetScreenCursor(Vector2 const& bufferCursor) const;

        public:
            Console();

            ConsoleState GetState() const;

            void SetWindowSize(int width, int height);

            void Open();
            void Close();

            void TypeCharacter(uint32_t unicodeCodepoint);
            void PressInsert();
            void PressBackspace();
            void PressDelete();
            void PressEnter();
            void MoveCursor(CursorMovement cursorMovement);
            void Focus();
            void ClearFocus();

            void Draw(ShaderProgram& shaderProgram) const;
            void Animate(float dt);
        };
    }
}