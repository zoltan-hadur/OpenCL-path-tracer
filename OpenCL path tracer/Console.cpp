#include "Console.h"

#include "Vector3.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Font.h"
#include "Character.h"
#include "Vertex.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        Console::Console()
        {
            _state = ConsoleState::Closed;
            _background = std::make_unique<Panel>(Vector2(0, 0), Vector2(1, 1), Color(0, 0.5f, 1, 0.68f));
            _text = std::make_unique<Text>(std::make_shared<Font>("C:/Windows/Fonts/cour.ttf", 16), "ASD\r\nqwe\r\nASD", Color(1, 1, 1));
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
        }

        void Console::SetWindowSize(int width, int height)
        {
            _positionAnimation.From = Vector2(0, -height);
            _positionAnimation.To = Vector2(0, 0);

            _background->SetSize(Vector2(width, height));
        }

        void Console::Open()
        {
            _state = ConsoleState::Opening;
            _positionAnimation.Start();
        }

        void Console::Close()
        {
            _state = ConsoleState::Closing;
            _positionAnimation.Start();
        }

        void Console::Toggle()
        {
            switch (_state)
            {
                case ConsoleState::Closed:
                    Open();
                    break;
                case ConsoleState::Open:
                    Close();
                    break;
            }
        }

        void Console::Draw(ShaderProgram& shaderProgram) const
        {
            if (_state == ConsoleState::Closed) return;
            shaderProgram.PushModelMatrix(Matrix4x4::TranslateMatrix({ _positionAnimation.GetCurrentValue(), 0.0f }));
            _background->Draw(shaderProgram);
            _text->Draw(shaderProgram);
            shaderProgram.PopModelMatrix();
        }

        void Console::Animate(float dt)
        {
            _positionAnimation.Animate(dt);
        }
    }
}