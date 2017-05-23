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
const int screen_width = 192 * 5;
const int screen_height = 108 * 5;
int keys_down[256];
float max_fps;

void onInitialization() {
	glViewport(0, 0, screen_width, screen_height);
	scene.init(screen_width, screen_height);
	console.init();
	console.print_help();
	max_fps = 60; GLConsole::cvars.attach_cvar<float>("app.max_fps", &max_fps, "Determines the maximum allowed frames per second. Interval: [30, infty).");

	//const int LAMP = scene.add_material(Material(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1), 0, 0, false));
	//const int RED = scene.add_material(Material(float3(1, 0, 0), float3(1, 1, 1), float3(0, 0, 0), 0, 50, false));

	//scene.add_sphere(Sphere(float3(800, 1000, 800), float3(0, 0, 0), 100, LAMP));
	//scene.add_sphere(Sphere(float3(800, 500, 800), float3(0, 0, 0), 100, RED));

	scene.add_sphere(Sphere(float3(800, 500, 500), float3(0, 0, 0), 200, 0));
	scene.add_sphere(Sphere(float3(800, 1000, 500), float3(0, 0, 0), 200, 1));
	scene.add_sphere(Sphere(float3(0, 1, 2), float3(3, 4, 5), 6, 7, 8));

	printf("%d\n", sizeof(Camera));

	scene.commit();
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
	} else {
		if (console.is_open()) {
			console.on_keyboard(key);
		} else {
			switch (key) {
				case ' ':
					glutFullScreenToggle();
					break;
				default:
					keys_down[key] = true;
					break;
			}
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
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			float X = (float)x;
			X = X / glutGet(GLUT_WINDOW_WIDTH)*screen_width;
			float Y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y);
			Y = Y / glutGet(GLUT_WINDOW_HEIGHT)*screen_height;
			//std::cout << cam.get_ray((int)X, (int)Y) << std::endl;
		}
	}
}

void onMouseMotion(int x, int y) {
	int dx = x - last_x;
	int dy = y - last_y;
	//cam.rotate(dy*0.10f, dx*0.10f);
	last_x = x;
	last_y = y;
}

Stopwatch watch;
float time_elapsed = 0;
int frames = 0;
float wait_time = 0;
float acc = 0;
void onIdle() {
	float dt = watch.get_delta_time();
	time_elapsed = time_elapsed + dt;
	acc = acc + dt;

	scene.render();


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
