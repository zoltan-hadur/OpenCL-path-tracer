#pragma once

#include <memory>

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

        class Console
        {
        private:
            ConsoleState _state;
            std::unique_ptr<Panel> _background;
            std::unique_ptr<Text> _text;
            Animation<Vector2> _positionAnimation;

        public:
            Console();

            void SetWindowSize(int width, int height);

            void Open();
            void Close();
            void Toggle();

            void Draw(ShaderProgram& shaderProgram) const;
            void Animate(float dt);
        };
    }
}