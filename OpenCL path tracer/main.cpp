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

using namespace OpenCL_PathTracer;
using namespace OpenCL_PathTracer::GL_Stuff;

int _width = 192 * 5;
int _height = 108 * 5;
GLFWwindow* _window = nullptr;
std::unique_ptr<ShaderProgram> _shaderProgram = nullptr;
Stopwatch _stopwatch;
std::shared_ptr<Font> _font = nullptr;
std::unique_ptr<Text> _fps = nullptr;
int _frames = 0;

void OnFramebufferSizeChanged(GLFWwindow* window, int width, int height)
{
    _width = width;
    _height = height;
    glViewport(0, 0, _width, _height);
    _shaderProgram->SetProjectionMatrix(Matrix4x4::OrthoProjectionMatrix(0, _width, _height, 0, -1, 1));
}

void Draw()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _fps->Bind();
    _fps->SetPosition({ 0, _height - _font->GetHeight() });
    auto time = _stopwatch.GetElapsedTime();
    if (time >= 0.5f)
    {
        _fps->SetValue(std::format("{:8.2f} FPS", _frames / time));
        _stopwatch.Restart();
        _frames = 0;
    }
    _fps->Draw(*_shaderProgram);
    _fps->Unbind();

    glfwSwapBuffers(_window);
    _frames++;
}

int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    _shaderProgram = std::make_unique<ShaderProgram>("default.vert", "default.frag");
    if (!_shaderProgram->IsLinkingSuccessful())
    {
        std::cout << _shaderProgram->GetLinkingLog() << std::endl;
    }
    _shaderProgram->Activate();

    _font = std::make_shared<Font>("C:/Windows/Fonts/cour.ttf", 16);
    _fps = std::make_unique<Text>(_font, "", Color(1, 1, 1));

    OnFramebufferSizeChanged(_window, _width, _height);
    _stopwatch.Start();
    while (!glfwWindowShouldClose(_window))
    {
        Draw();
        glfwPollEvents();
    }

    glfwDestroyWindow(_window);
    glfwTerminate();
    return 0;
}
