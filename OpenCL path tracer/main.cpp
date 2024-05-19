#define GLUT_KEY_ESC 27

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Stopwatch.h"
#include "Vector3.h"
#include "Bitmap.h"
#include "Vertex.h"
#include "Matrix4x4.h"
#include "Texture.h"
#include "Character.h"
#include "Font.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Text.h"
#include "ShaderMode.h"
#include "FlagHelper.h"
#include "FileHelper.h"
#include "StringHelper.h"
#include "Panel.h"
#include "Console.h"
#include "EasingFunction.h"

using namespace OpenCL_PathTracer;
using namespace OpenCL_PathTracer::GL_Stuff;

class TexturedQuad : public Component
{
public:
    TexturedQuad() : Component(std::make_shared<Texture>(Bitmap::Read("rendered/image_0.bmp")))
    {
        ReplaceData(
            {
            { {       0,       0 }, { 0.0f, 1.0f } },
            { {       0, 108 * 2 }, { 0.0f, 0.0f } },
            { { 192 * 2, 108 * 2 }, { 1.0f, 0.0f } },
            { { 192 * 2,       0 }, { 1.0f, 1.0f } }
            },
            {
                0, 1, 2,
                2, 3, 0
            });
    }
};

int _width = 192 * 5;
int _height = 108 * 5;
GLFWmonitor* _monitor = nullptr;
GLFWvidmode const* _videoMode = nullptr;
GLFWwindow* _window = nullptr;
std::unique_ptr<ShaderProgram> _shaderProgram = nullptr;
Stopwatch _stopwatch;
std::shared_ptr<Font> _font = nullptr;
std::unique_ptr<Text> _fps = nullptr;
std::unique_ptr<Console> _console = nullptr;
int _frames = 0;
bool _isFullScreen = false;
int _windowPositionX = 0;
int _windowPositionY = 0;
int _windowWidth = 0;
int _windowHeight = 0;
std::unique_ptr<TexturedQuad> _quad = nullptr;
Stopwatch _watch = Stopwatch::StartNew();

void Draw();

void OnFramebufferSizeChanged(GLFWwindow* window, int width, int height)
{
    _width = width;
    _height = height;
    glViewport(0, 0, _width, _height);
    _shaderProgram->SetProjectionMatrix(Matrix4x4::OrthoProjectionMatrix(0, _width, _height, 0, -1, 1));

    Vector2 position, size;
    if (192.0f / 108.0f < (float)_width / (float)_height)
    {
        size = { _height * 192.0f / 108.f, (float)_height };
        position = { (_width - size.x) / 2.0f, 0.0f };
    }
    else
    {
        size = { (float)_width, _width / 192.0f * 108.0f };
        position = { 0.0f, (_height - size.y) / 2.0f };
    }
    _quad->SetPosition(position);
    _quad->SetSize(size);

    _fps->SetPosition({ 0, _height - _font->GetHeight() });

    _console->SetWindowSize(width, height);

    Draw();
}

void OnKeyReceived(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && FlagHelper::IsFlagSet(mods, GLFW_MOD_ALT))
    {
        if (_isFullScreen)
        {
            glfwSetWindowMonitor(window, nullptr, _windowPositionX, _windowPositionY, _windowWidth, _windowHeight, 0);
        }
        else
        {
            glfwGetWindowPos(window, &_windowPositionX, &_windowPositionY);
            glfwGetWindowSize(window, &_windowWidth, &_windowHeight);
            glfwSetWindowMonitor(window, _monitor, 0, 0, _videoMode->width, _videoMode->height, _videoMode->refreshRate);
        }
        _isFullScreen = !_isFullScreen;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(_window, GL_TRUE);
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        _console->Toggle();
    }
}

void OnCharacterReceived(GLFWwindow* window, unsigned int codepoint)
{

}

void Draw()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _quad->Draw(*_shaderProgram);

    _console->Animate(_watch.GetDeltaTime());
    _console->Draw(*_shaderProgram);

    auto time = _stopwatch.GetElapsedTime();
    if (time >= 0.5f)
    {
        _fps->SetValue(std::format("{0:8.2f} FPS", _frames / time));
        _stopwatch.Restart();
        _frames = 0;
    }
    _fps->Draw(*_shaderProgram);

    glfwSwapBuffers(_window);
    _frames++;
}

int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _monitor = glfwGetPrimaryMonitor();
    _videoMode = glfwGetVideoMode(_monitor);

    glfwWindowHint(GLFW_RED_BITS, _videoMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, _videoMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, _videoMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, _videoMode->refreshRate);

    _window = glfwCreateWindow(_width, _height, "OpenCL path tracer", nullptr, nullptr);
    if (!_window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(_window);
    gladLoadGL();
    glfwSetFramebufferSizeCallback(_window, OnFramebufferSizeChanged);
    glfwSetKeyCallback(_window, OnKeyReceived);
    glfwSetCharCallback(_window, OnCharacterReceived);

    _shaderProgram = std::make_unique<ShaderProgram>("default.vert", "default.frag");
    if (!_shaderProgram->IsLinkingSuccessful())
    {
        std::cout << _shaderProgram->GetLinkingLog() << std::endl;
    }
    _shaderProgram->Activate();

    _font = std::make_shared<Font>("C:/Windows/Fonts/cour.ttf", 16);
    _fps = std::make_unique<Text>(_font, "", Color(1, 1, 1));
    _quad = std::make_unique<TexturedQuad>();
    _console = std::make_unique<Console>();

    OnFramebufferSizeChanged(_window, _width, _height);
    _stopwatch.Start();
    while (!glfwWindowShouldClose(_window))
    {
        Draw();
        glfwPollEvents();
        _sleep(1);
    }

    glfwDestroyWindow(_window);
    glfwTerminate();
    return 0;
}
