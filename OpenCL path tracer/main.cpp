#define GLUT_KEY_ESC 27

#include <GL\glew.h>
#include <GL\glext.h>
#include <GL\freeglut.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <utility>
#include <chrono>
#include <thread>
#include "Scene.h"
#include "GLConsole.h"

GLConsole console;
Scene scene;
Stopwatch watch;
const int screen_width = 192 * 5;
const int screen_height = 108 * 5;
int keys_down[256];
float max_fps;

void onInitialization() {
	glViewport(0, 0, screen_width, screen_height);
	scene.init(screen_width, screen_height);
	console.init();
	console.print_help();
	max_fps = 30; GLConsole::cvars.attach_cvar<float>("app.max_fps", &max_fps, "Determines the maximum allowed frames per second. Interval: [30, infty).");

	const int lamp = scene.add_material(new Material(float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), float3(8, 6, 4) * 1, 0, 0, 0));
	const int d_red = scene.add_material(new Material(float3(1.0, 0.3, 0.3), float3(0.04, 0.04, 0.04), float3(0, 0, 0), 2, 1, 1));
	const int d_green = scene.add_material(new Material(float3(0.3, 1.0, 0.3), float3(0.04, 0.04, 0.04), float3(0, 0, 0), 2, 1, 1));
	const int d_blue = scene.add_material(new Material(float3(0.3, 0.3, 1.0), float3(0.04, 0.04, 0.04), float3(0, 0, 0), 2, 1, 1));
	const int d_white = scene.add_material(new Material(float3(1.0, 1.0, 1.0), float3(0.04, 0.04, 0.04), float3(0, 0, 0), 2, 1, 1));
	const int s_red = scene.add_material(new Material(float3(1.0, 0.3, 0.3), float3(1.0, 0.3, 0.3), float3(0, 0, 0), 2, 0, 1));
	const int s_green = scene.add_material(new Material(float3(0.3, 1.0, 0.3), float3(0.3, 1.0, 0.3), float3(0, 0, 0), 2, 0, 1));
	const int s_blue = scene.add_material(new Material(float3(0.3, 0.3, 1.0), float3(0.3, 0.3, 1.0), float3(0, 0, 0), 2, 0.01, 1));
	const int s_white = scene.add_material(new Material(float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), float3(0, 0, 0), 2.00, 0.000, 1));
	const int gold = scene.add_material(new Material(float3(0.0, 0.0, 0.0), float3(252, 201, 88) / 255, float3(0, 0, 0), 0.16, 0.0, 1));
	const int aluminium = scene.add_material(new Material(float3(0.0, 0.0, 0.0), float3(128, 126, 121) / 255, float3(0, 0, 0), 0.16, 0.0, 1));
	const int glass = scene.add_material(new Material(float3(0.0, 0.0, 0.0), float3(1, 1, 1), float3(0, 0, 0), 1.5, 0.0, 2));

	const int earth = scene.add_texture("textures/earth_2k.bmp");
	const int squares = scene.add_texture("textures/squares_2k.bmp");
	const int ball = scene.add_texture("textures/ball_14_1k.bmp");
	const int tex_earth = scene.add_texture_info(earth, 1);
	const int bump_earth = scene.add_texture_info(earth, 2);
	const int full_earth = scene.add_texture_info(earth, 1 | 2);
	const int bump_squares = scene.add_texture_info(squares, 2);
	const int full_ball = scene.add_texture_info(ball, 1 | 2);

	// Lamp
	scene.add_triangle(new Triangle(float3(600.0f, 999.99f, 0.0f), float3(1000.0f, 999.99f, 0.0f), float3(1000.0f, 999.99f, 400.0f), lamp));
	scene.add_triangle(new Triangle(float3(600.0f, 999.99f, 0.0f), float3(600.0f, 999.99f, 400.0f), float3(1000.0f, 999.99f, 400.0f), lamp));

	//left wall
	scene.add_triangle(new Triangle(float3(0.0f, 0.0f, -2000.0f), float3(0.0f, 0.0f, 1000.0f), float3(0.0f, 1000.0f, -2000.0f), d_red));
	scene.add_triangle(new Triangle(float3(0.0f, 1000.0f, 1000.0f), float3(0.0f, 1000.0f, -2000.0f), float3(0.0f, 0.0f, 1000.0f), d_red));
	//right wall
	scene.add_triangle(new Triangle(float3(1600.0f, 0.0f, -2000.0f), float3(1600.0f, 0.0f, 1000.0f), float3(1600.0f, 1000.0f, -2000.0f), d_blue));
	scene.add_triangle(new Triangle(float3(1600.0f, 1000.0f, 1000.0f), float3(1600.0f, 1000.0f, -2000.0f), float3(1600.0f, 0.0f, 1000.0f), d_blue));
	//floor
	scene.add_triangle(new Triangle(float3(0.0f, 0.0f, -2000.0f), float3(1600.0f, 0.0f, -2000.0f), float3(1600.0f, 0.0f, 1000.0f), d_white));
	scene.add_triangle(new Triangle(float3(0.0f, 0.0f, -2000.0f), float3(1600.0f, 0.0f, 1000.0f), float3(0.0f, 0.0f, 1000.0f), d_white));
	//ceiling
	scene.add_triangle(new Triangle(float3(0.0f, 1000.0f, -2000.0f), float3(1600.0f, 1000.0f, -2000.0f), float3(1600.0f, 1000.0f, 1000.0f), d_white));
	scene.add_triangle(new Triangle(float3(0.0f, 1000.0f, -2000.0f), float3(1600.0f, 1000.0f, 1000.0f), float3(0.0f, 1000.0f, 1000.0f), d_white));
	//front wall
	scene.add_triangle(new Triangle(float3(1600.0f, 0.0f, 1000.0), float3(0.0f, 0.0f, 1000.0f), float3(1600.0f, 1000.0f, 1000.0f), d_white));
	scene.add_triangle(new Triangle(float3(0.0f, 0.0f, 1000.0f), float3(1600.0f, 1000.0f, 1000.0f), float3(0.0f, 1000.0f, 1000.0f), d_white));
	//rear wall
	scene.add_triangle(new Triangle(float3(0.0f, 0.0f, -2000.0f), float3(1600.0f, 0.0f, -2000.0f), float3(1600.0f, 1000.0f, -2000.0f), d_white));
	scene.add_triangle(new Triangle(float3(0.0f, 0.0f, -2000.0f), float3(1600.0f, 1000.0f, -2000.0f), float3(0.0f, 1000.0f, -2000.0f), d_white));


	////scene.add_sphere(new Sphere(float3(500, 100, 400), float3(0, 0, 0), 100, s_blue));
	//scene.add_sphere(new Sphere(float3(400, 200, 700), float3(0, 0, 0), 200, s_green));
	////scene.add_sphere(new Sphere(float3(1450, 150, 850), float3(0, 0, 0), 150, s_red));
	////scene.add_sphere(new Sphere(float3(800, 200, 500), float3(0, 0, 0), 200, gold));
	//scene.add_sphere(new Sphere(float3(1200, 200, 200), float3(0, 0, 0), 200, glass));

	//scene.add_sphere(new Sphere(float3(150, 150, -100), float3(90, 0, 0), 150, glass, full_ball));

	////scene.add_sphere(new Sphere(float3(900, 100, -200), float3(0, 0, 0), 100, glass));
	//scene.add_sphere(new Sphere(float3(500 - 1, 150, 500), float3(180, 0, 0), 150, aluminium, bump_squares));
	//scene.add_sphere(new Sphere(float3(800, 150, 450), float3(180, 0, 0), 150, gold, bump_earth));
	//scene.add_sphere(new Sphere(float3(1100 + 1, 150, 400), float3(180, 0, 0), 150, s_white, full_earth));

	float R = 150;
	float r = R / 2;
	//scene.add_sphere(new Sphere(float3(800, 1000, 500), float3(0, 0, 0), 100, lamp));
	//scene.add_sphere(new Sphere(float3(800, r, 500), float3(0, 0, 0), r, lamp));

	scene.add_sphere(new Sphere(float3(800 - R - 1, R, 500 - R - 1), float3(180, 0, 0), R, gold, bump_earth));
	scene.add_sphere(new Sphere(float3(800 + R + 1, R, 500 - R - 1), float3(180, 0, 0), R, gold, full_earth));
	scene.add_sphere(new Sphere(float3(800 - R - 1, R, 500 + R + 1), float3(180, 0, 0), R, d_green, bump_earth));
	scene.add_sphere(new Sphere(float3(800 + R + 1, R, 500 + R + 1), float3(180, 0, 0), R, aluminium, bump_squares));
	scene.add_sphere(new Sphere(float3(800, R + sqrt(2)*R, 500), float3(45, 0, 0), R, s_white, full_ball));

	scene.commit();
	watch.start();
}

void onDisplay() {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.draw();
	console.draw();

	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
	int modifier = glutGetModifiers();
	if (modifier & GLUT_ACTIVE_SHIFT) {
		console.shift_pressed();
	} else {
		console.shift_released();
	}
	if (key == GLUT_KEY_ESC) {
		glutLeaveFullScreen();
		glutDestroyWindow(1);
		exit(EXIT_SUCCESS);
	}
	if (console.is_open()) {
		console.on_keyboard(key);
	} else {
		switch (key) {
			case ' ':
				glutFullScreenToggle();
				break;
			case 't':
				scene.next_tone_map();
				break;
			case 'm':
				scene.next_mode();
				break;
			case 'f':
				scene.next_filter();
				break;
			case '+':
				scene.inc_depth();
				break;
			case '-':
				scene.dec_depth();
				break;
			default:
				keys_down[key] = true;
				break;
		}
	}
}

void onKeyboardUp(unsigned char key, int x, int y) {
	keys_down[key] = false;
}

void onSpecial(int key, int x, int y) {
	int modifier = glutGetModifiers();
	if (modifier & GLUT_ACTIVE_SHIFT) {
		console.shift_pressed();
	} else {
		console.shift_released();
	}

	if (key == GLUT_KEY_F1) {
		console.toggle();
	} else {
		if (console.is_open()) {
			console.on_special(key);
		} else {
			switch (key) {

			}
		}
	}
}

void onSpecialUp(int key, int x, int y) {
	int modifier = glutGetModifiers();
	if (modifier & GLUT_ACTIVE_SHIFT) {
		console.shift_pressed();
	} else {
		console.shift_released();
	}
}

int last_x, last_y;
void onMouse(int button, int state, int x, int y) {
	if ((button == 3) || (button == 4)) {	// Mouse wheel event
		if (state == GLUT_UP) return;
		if (console.is_open()) {
			(button == 3) ? console.scroll_up() : console.scroll_down();
		}
	} else {	// Normal click event
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

void onMouseMotion(int x, int y) {
	int dx = x - last_x;
	int dy = y - last_y;
	scene.get_camera().rotate(dy*0.10f, dx*0.10f);
	last_x = x;
	last_y = y;
}

int frames = 0;
float wait_time = 0;
float time_elapsed = 0;
float acc = 0;
void onIdle() {
	float dt = watch.get_delta_time();
	time_elapsed = time_elapsed + dt;
	acc = acc + dt;

	if (keys_down['w']) {
		scene.get_camera().translate_forward(dt * 1000);
	}
	if (keys_down['s']) {
		scene.get_camera().translate_forward(-dt * 1000);
	}
	if (keys_down['d']) {
		scene.get_camera().translate_right(dt * 1000);
	}
	if (keys_down['a']) {
		scene.get_camera().translate_right(-dt * 1000);
	}
	if (keys_down['q']) {
		scene.get_camera().translate_up(dt * 1000);
	}
	if (keys_down['y']) {
		scene.get_camera().translate_up(-dt * 1000);
	}

	scene.render();
	console.process();

	if (frames >= 1) {
		wait_time = std::max((1.0f - (time_elapsed * max_fps / 1 - max_fps * wait_time)) / max_fps, 0.0f);
		frames = 0;
		time_elapsed = 0;
	}
	if (acc > wait_time) {
		glutPostRedisplay();
		frames++;
		acc = 0;
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("OpenCL path tracer");
  glewInit();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	onInitialization();

	glutDisplayFunc(onDisplay);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouseMotion);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutSpecialFunc(onSpecial);
	glutSpecialUpFunc(onSpecialUp);

	glutMainLoop();

	return 0;
}
