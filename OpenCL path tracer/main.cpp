#define GLUT_KEY_ESC 27

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <GL\glext.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <utility>
#include <chrono>
#include <thread>
//#include "Scene.h"
//#include "GLConsole.h"
#include "TextureType.h"
#include "Stopwatch.h"
#include "Vector2.h"
#include "Bitmap.h"
#include "Vertex.h"
#include "Matrix4x4.h"
#include <numbers>
#include "Texture.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "Font.h"
#include "FileHelper.h"
#include <map>
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

//GLConsole console;
//Scene scene;
Stopwatch watch;
const int screen_width = 192 * 5;
const int screen_height = 108 * 5;
int keys_down[256];
float max_fps;

void onInitialization()
{
    glViewport(0, 0, screen_width, screen_height);
    //scene.init(screen_width, screen_height);
    //console.init();
    //console.print_help();
    //max_fps = 30; GLConsole::cvars.Add(ConsoleVariable(&max_fps, "app.max_fps", "Determines the maximum allowed frames per second. Interval: [30, infty)."));

    //const int lamp = scene.add_material(new Material(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 0.0), Vector3(8, 6, 4) * 1, 0, 0, 0));
    //const int d_red = scene.add_material(new Material(Vector3(1.0, 0.3, 0.3), Vector3(0.04, 0.04, 0.04), Vector3(0, 0, 0), 2, 1, 1));
    //const int d_green = scene.add_material(new Material(Vector3(0.3, 1.0, 0.3), Vector3(0.04, 0.04, 0.04), Vector3(0, 0, 0), 2, 1, 1));
    //const int d_blue = scene.add_material(new Material(Vector3(0.3, 0.3, 1.0), Vector3(0.04, 0.04, 0.04), Vector3(0, 0, 0), 2, 1, 1));
    //const int d_white = scene.add_material(new Material(Vector3(1.0, 1.0, 1.0), Vector3(0.04, 0.04, 0.04), Vector3(0, 0, 0), 2, 1, 1));
    //const int s_red = scene.add_material(new Material(Vector3(1.0, 0.3, 0.3), Vector3(1.0, 0.3, 0.3), Vector3(0, 0, 0), 2, 0, 1));
    //const int s_green = scene.add_material(new Material(Vector3(0.3, 1.0, 0.3), Vector3(0.3, 1.0, 0.3), Vector3(0, 0, 0), 2, 0, 1));
    //const int s_blue = scene.add_material(new Material(Vector3(0.3, 0.3, 1.0), Vector3(0.3, 0.3, 1.0), Vector3(0, 0, 0), 2, 0.01, 1));
    //const int s_white = scene.add_material(new Material(Vector3(1.0, 1.0, 1.0), Vector3(1.0, 1.0, 1.0), Vector3(0, 0, 0), 2.00, 0.000, 1));
    //const int gold = scene.add_material(new Material(Vector3(0.0, 0.0, 0.0), Vector3(252, 201, 88) / 255, Vector3(0, 0, 0), 0.16, 0.0, 1));
    //const int aluminium = scene.add_material(new Material(Vector3(0.0, 0.0, 0.0), Vector3(128, 126, 121) / 255, Vector3(0, 0, 0), 0.16, 0.0, 1));
    //const int glass = scene.add_material(new Material(Vector3(0.0, 0.0, 0.0), Vector3(1, 1, 1), Vector3(0, 0, 0), 1.5, 0.0, 2));

    //const int earth = scene.add_texture("textures/earth_2k.bmp");
    //const int squares = scene.add_texture("textures/squares_2k.bmp");
    //const int ball = scene.add_texture("textures/ball_14_1k.bmp");
    //const int tex_earth = scene.add_texture_info(earth, TextureType::Texture);
    //const int bump_earth = scene.add_texture_info(earth, TextureType::BumpMap);
    //const int full_earth = scene.add_texture_info(earth, TextureType::Texture | TextureType::BumpMap);
    //const int bump_squares = scene.add_texture_info(squares, TextureType::BumpMap);
    //const int full_ball = scene.add_texture_info(ball, TextureType::Texture | TextureType::BumpMap);

    //// Lamp
    //scene.add_triangle(new Triangle(Vector3(600.0f, 999.99f, 0.0f), Vector3(1000.0f, 999.99f, 0.0f), Vector3(1000.0f, 999.99f, 400.0f), lamp));
    //scene.add_triangle(new Triangle(Vector3(600.0f, 999.99f, 0.0f), Vector3(600.0f, 999.99f, 400.0f), Vector3(1000.0f, 999.99f, 400.0f), lamp));

    ////left wall
    //scene.add_triangle(new Triangle(Vector3(0.0f, 0.0f, -2000.0f), Vector3(0.0f, 0.0f, 1000.0f), Vector3(0.0f, 1000.0f, -2000.0f), d_red));
    //scene.add_triangle(new Triangle(Vector3(0.0f, 1000.0f, 1000.0f), Vector3(0.0f, 1000.0f, -2000.0f), Vector3(0.0f, 0.0f, 1000.0f), d_red));
    ////right wall
    //scene.add_triangle(new Triangle(Vector3(1600.0f, 0.0f, -2000.0f), Vector3(1600.0f, 0.0f, 1000.0f), Vector3(1600.0f, 1000.0f, -2000.0f), d_blue));
    //scene.add_triangle(new Triangle(Vector3(1600.0f, 1000.0f, 1000.0f), Vector3(1600.0f, 1000.0f, -2000.0f), Vector3(1600.0f, 0.0f, 1000.0f), d_blue));
    ////floor
    //scene.add_triangle(new Triangle(Vector3(0.0f, 0.0f, -2000.0f), Vector3(1600.0f, 0.0f, -2000.0f), Vector3(1600.0f, 0.0f, 1000.0f), d_white));
    //scene.add_triangle(new Triangle(Vector3(0.0f, 0.0f, -2000.0f), Vector3(1600.0f, 0.0f, 1000.0f), Vector3(0.0f, 0.0f, 1000.0f), d_white));
    ////ceiling
    //scene.add_triangle(new Triangle(Vector3(0.0f, 1000.0f, -2000.0f), Vector3(1600.0f, 1000.0f, -2000.0f), Vector3(1600.0f, 1000.0f, 1000.0f), d_white));
    //scene.add_triangle(new Triangle(Vector3(0.0f, 1000.0f, -2000.0f), Vector3(1600.0f, 1000.0f, 1000.0f), Vector3(0.0f, 1000.0f, 1000.0f), d_white));
    ////front wall
    //scene.add_triangle(new Triangle(Vector3(1600.0f, 0.0f, 1000.0), Vector3(0.0f, 0.0f, 1000.0f), Vector3(1600.0f, 1000.0f, 1000.0f), d_white));
    //scene.add_triangle(new Triangle(Vector3(0.0f, 0.0f, 1000.0f), Vector3(1600.0f, 1000.0f, 1000.0f), Vector3(0.0f, 1000.0f, 1000.0f), d_white));
    ////rear wall
    //scene.add_triangle(new Triangle(Vector3(0.0f, 0.0f, -2000.0f), Vector3(1600.0f, 0.0f, -2000.0f), Vector3(1600.0f, 1000.0f, -2000.0f), d_white));
    //scene.add_triangle(new Triangle(Vector3(0.0f, 0.0f, -2000.0f), Vector3(1600.0f, 1000.0f, -2000.0f), Vector3(0.0f, 1000.0f, -2000.0f), d_white));


    //////scene.add_sphere(new Sphere(Vector3(500, 100, 400), Vector3(0, 0, 0), 100, s_blue));
    ////scene.add_sphere(new Sphere(Vector3(400, 200, 700), Vector3(0, 0, 0), 200, s_green));
    //////scene.add_sphere(new Sphere(Vector3(1450, 150, 850), Vector3(0, 0, 0), 150, s_red));
    //////scene.add_sphere(new Sphere(Vector3(800, 200, 500), Vector3(0, 0, 0), 200, gold));
    ////scene.add_sphere(new Sphere(Vector3(1200, 200, 200), Vector3(0, 0, 0), 200, glass));

    ////scene.add_sphere(new Sphere(Vector3(150, 150, -100), Vector3(90, 0, 0), 150, glass, full_ball));

    //////scene.add_sphere(new Sphere(Vector3(900, 100, -200), Vector3(0, 0, 0), 100, glass));
    ////scene.add_sphere(new Sphere(Vector3(500 - 1, 150, 500), Vector3(180, 0, 0), 150, aluminium, bump_squares));
    ////scene.add_sphere(new Sphere(Vector3(800, 150, 450), Vector3(180, 0, 0), 150, gold, bump_earth));
    ////scene.add_sphere(new Sphere(Vector3(1100 + 1, 150, 400), Vector3(180, 0, 0), 150, s_white, full_earth));

    //float R = 150;
    //float r = R / 2;
    ////scene.add_sphere(new Sphere(Vector3(800, 1000, 500), Vector3(0, 0, 0), 100, lamp));
    ////scene.add_sphere(new Sphere(Vector3(800, r, 500), Vector3(0, 0, 0), r, lamp));

    //scene.add_sphere(new Sphere(Vector3(800 - R - 1, R, 500 - R - 1), Vector3(180, 0, 0), R, gold, bump_earth));
    //scene.add_sphere(new Sphere(Vector3(800 + R + 1, R, 500 - R - 1), Vector3(180, 0, 0), R, gold, full_earth));
    //scene.add_sphere(new Sphere(Vector3(800 - R - 1, R, 500 + R + 1), Vector3(180, 0, 0), R, d_green, bump_earth));
    //scene.add_sphere(new Sphere(Vector3(800 + R + 1, R, 500 + R + 1), Vector3(180, 0, 0), R, aluminium, bump_squares));
    //scene.add_sphere(new Sphere(Vector3(800, R + sqrt(2)*R, 500), Vector3(45, 0, 0), R, s_white, full_ball));

    //scene.commit();
    watch.Start();
}

void onDisplay()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Top left is (0, 0), x goes right, y goes down
    //auto width = glutGet(GLUT_WINDOW_WIDTH);
    //auto height = glutGet(GLUT_WINDOW_HEIGHT);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(0, width, 0, height, -1, 1);
    //glScalef(1, -1, 1);
    //glTranslatef(0, -height, 0);

    //scene.draw();
    //console.draw();

    //glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y)
{
    //int modifier = glutGetModifiers();
    //if (modifier & GLUT_ACTIVE_SHIFT)
    //{
    //    //console.shift_pressed();
    //}
    //else
    //{
    //    //console.shift_released();
    //}
    //if (key == GLUT_KEY_ESC)
    //{
    //    glutLeaveFullScreen();
    //    glutDestroyWindow(1);
    //    exit(EXIT_SUCCESS);
    //}
    //if (console.is_open())
    //{
    //    console.on_keyboard(key);
    //}
    //else
    //{
    //    switch (key)
    //    {
    //        case ' ':
    //            glutFullScreenToggle();
    //            break;
    //        case 't':
    //            scene.next_tone_map();
    //            break;
    //        case 'm':
    //            scene.next_mode();
    //            break;
    //        case 'f':
    //            scene.next_filter();
    //            break;
    //        case '+':
    //            scene.inc_depth();
    //            break;
    //        case '-':
    //            scene.dec_depth();
    //            break;
    //        default:
    //            keys_down[key] = true;
    //            break;
    //    }
    //}
}

void onKeyboardUp(unsigned char key, int x, int y)
{
    keys_down[key] = false;
}

void onSpecial(int key, int x, int y)
{
    //int modifier = glutGetModifiers();
    //if (modifier & GLUT_ACTIVE_SHIFT)
    //{
    //    console.shift_pressed();
    //}
    //else
    //{
    //    console.shift_released();
    //}

    //if (key == GLUT_KEY_F1)
    //{
    //    console.toggle();
    //}
    //else
    //{
    //    if (console.is_open())
    //    {
    //        console.on_special(key);
    //    }
    //    else
    //    {
    //        switch (key)
    //        {

    //        }
    //    }
    //}
}

void onSpecialUp(int key, int x, int y)
{
    //int modifier = glutGetModifiers();
    //if (modifier & GLUT_ACTIVE_SHIFT)
    //{
    //    //console.shift_pressed();
    //}
    //else
    //{
    //    //console.shift_released();
    //}
}

int last_x, last_y;
void onMouse(int button, int state, int x, int y)
{
    if ((button == 3) || (button == 4))
    {	// Mouse wheel event
        //if (state == GLUT_UP) return;
        //if (console.is_open())
        //{
        //    (button == 3) ? console.scroll_up() : console.scroll_down();
        //}
    }
    else
    {	// Normal click event
        last_x = x; last_y = y;
        //if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        //	float X = (float)x;
        //	X = X / glutGet(GLUT_WINDOW_WIDTH)*screen_width;
        //	float Y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y);
        //	Y = Y / glutGet(GLUT_WINDOW_HEIGHT)*screen_height;
        //	//std::cout << cam.get_ray((int)X, (int)Y) << std::endl;
        //}
    }
}

void onMouseMotion(int x, int y)
{
    int dx = x - last_x;
    int dy = y - last_y;
    //scene.get_camera().rotate(dy * 0.10f, dx * 0.10f);
    last_x = x;
    last_y = y;
}

int frames = 0;
float wait_time = 0;
float time_elapsed = 0;
float acc = 0;
void onIdle()
{
    float dt = watch.GetDeltaTime();
    time_elapsed = time_elapsed + dt;
    acc = acc + dt;

    //if (keys_down['w'])
    //{
    //    scene.get_camera().translate_forward(dt * 1000);
    //}
    //if (keys_down['s'])
    //{
    //    scene.get_camera().translate_forward(-dt * 1000);
    //}
    //if (keys_down['d'])
    //{
    //    scene.get_camera().translate_right(dt * 1000);
    //}
    //if (keys_down['a'])
    //{
    //    scene.get_camera().translate_right(-dt * 1000);
    //}
    //if (keys_down['q'])
    //{
    //    scene.get_camera().translate_up(dt * 1000);
    //}
    //if (keys_down['y'])
    //{
    //    scene.get_camera().translate_up(-dt * 1000);
    //}

    //scene.render();
    //console.process();

    if (frames >= 1)
    {
        wait_time = std::max((1.0f - (time_elapsed * max_fps / 1 - max_fps * wait_time)) / max_fps, 0.0f);
        frames = 0;
        time_elapsed = 0;
    }
    if (acc > wait_time)
    {
        //glutPostRedisplay();
        frames++;
        acc = 0;
    }
}

int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(screen_width, screen_height, "OpenCL path tracer", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, screen_width, screen_height);

    // Vertices coordinates
    auto vertices = std::vector<Vertex>
    {
        { {       0,       0 }, { 0.0f, 1.0f } },
        { {       0, 108 * 2 }, { 0.0f, 0.0f } },
        { { 192 * 2, 108 * 2 }, { 1.0f, 0.0f } },
        { { 192 * 2,       0 }, { 1.0f, 1.0f } }
    };

    // Indices for vertices order
    auto indices = std::vector<GLuint>
    {
        0, 1, 2,
        2, 3, 0
    };

    auto texture = Texture(Bitmap::Read("rendered/image_0.bmp"));

    auto shaderProgram = ShaderProgram("default.vert", "default.frag");
    auto vao1 = VAO();
    vao1.Bind();

    auto vbo1 = VBO(vertices);
    vbo1.Bind();
    auto ebo1 = EBO(indices);
    ebo1.Bind();

    vao1.LinkAttrib(vbo1, 0, 4, GL_FLOAT, sizeof(float) * 4, (void*)0);
    vao1.LinkAttrib(vbo1, 1, 4, GL_FLOAT, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    vao1.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    // C:/Windows/Fonts/cour.ttf
    auto font = Font("C:/Windows/Fonts/cour.ttf", 16);

    auto size = font.MeasureText("The quick brown fox jumps over the lazy dog");

    watch.Start();
    int cntr = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        shaderProgram.ProjectionMatrix(Matrix4x4::OrthoProjectionMatrix(0, screen_width, screen_height, 0, -1, 1));

        shaderProgram.Mode(Mode::Texture);
        shaderProgram.ModelMatrix(Matrix4x4::IdentityMatrix().Scale({ 2, 2, 2 }).Translate({ 100, 50, 0 }));
        texture.Bind();
        vao1.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        texture.Unbind();
        vao1.Unbind();

        // The quick brown fox jumps over the lazy dog

        shaderProgram.ModelMatrix(Matrix4x4::IdentityMatrix().Translate({ 0, 0, 0 }));
        shaderProgram.Mode(Mode::Text);
        shaderProgram.Color(Color(1, 1, 1));

        font.Bind();
        for (int i = 0; i < 5; ++i)
        {
            for (auto c : "The quick brown fox jumps over the lazy dog")
            {
                if (c == '\0')
                {
                    continue;
                }
                auto advance = font.Draw(c);

                shaderProgram.ModelMatrix(shaderProgram.ModelMatrix().Translate({ advance, 0, 0 }));
            }
            shaderProgram.ModelMatrix(Matrix4x4::IdentityMatrix().Translate({ 0, 0 + font.Height() * (i + 1), 0}));
        }
        font.Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();

        _sleep(10);
        //std::cout << cntr++ / watch.GetElapsedTime() << std::endl;
    }

    font.Delete();

    texture.Delete();
    vao1.Delete();
    vbo1.Delete();
    ebo1.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
