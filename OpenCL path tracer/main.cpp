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
#include "OpenCL_Device.h"
#include "GLConsole.h"
#include "float3.h"
#include "Ray.h"
#include "Camera.h"

GLConsole console;
OpenCL_Device device;
const int screen_width = 192 * 5;
const int screen_height = 108 * 5;
int keys_down[256];
float max_fps;

void capture_picture() {
	static int id = 0;
	std::string file_name = "image_" + std::to_string(id) + ".bmp";
	FILE* file;
	std::vector<cl_uchar> data = std::vector<cl_uchar>(3 * screen_width*screen_height);
	int file_size = 54 + 3 * screen_width * screen_height;

	glBindTexture(GL_TEXTURE_2D, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, data.data());

	unsigned char file_header[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
	unsigned char info_header[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };
	unsigned char pad[3] = { 0, 0, 0 };

	*(int*)&(file_header[2]) = file_size;
	*(int*)&(info_header[4]) = screen_width;
	*(int*)&(info_header[8]) = screen_height;

	file = fopen(file_name.c_str(), "w+");
	fwrite(file_header, 1, 14, file);
	fwrite(info_header, 1, 40, file);
	for (int i = 0; i < screen_height; i++) {
		fwrite(data.data() + (screen_width*(screen_height - i - 1) * 3), 3, screen_width, file);
		fwrite(pad, 1, (4 - (screen_width * 3) % 4) % 4, file);
	}
	fclose(file);
	id++;
	GLConsole::cout << file_name + " created\n";
}

void onInitialization() {
	glViewport(0, 0, screen_width, screen_height);

	device.init(screen_width, screen_height);
	//std::cout << device << std::endl;
	console.init();
	console.add_function("capture_picture()", capture_picture);
	console.print_help();
	max_fps = 60; console.variables().attach_cvar<float>("app.max_fps", &max_fps, "Determines the maximum allowed frames per second. Interval: [30, infty).");


	std::string path = "textures/earth_2k.bmp";
	unsigned char header[54]; unsigned int dataPos; unsigned int width, height; unsigned int imageSize; //unsigned char * data;
	std::vector<cl_uchar> data;
	FILE* file = fopen(path.c_str(), "rb");
	if (!file) {
		printf("Image could not be opened\n\r");
		exit(1);
	}
	if (fread(header, 1, 54, file) != 54) {// If not 54 bytes read : problem
		printf("Not a correct BMP file\n\r");
		exit(2);
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n\r");
		exit(3);
	}
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (width > 2048 || height > 1024) {
		printf("Texture resolution is too big\n\r");
		exit(4);
	}
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
	//data = new unsigned char[imageSize];
	data.resize(imageSize);
	fread(data.data(), 1, imageSize, file);
	fclose(file);

	std::vector<cl_float> rgb = device.bgr_to_rgb(data, width, height);
	std::vector<cl_float> grayscale = device.rgb_to_grayscale(rgb, width, height);
	std::vector<cl_float> bump_map = device.derivate_image(grayscale, width, height);
	std::vector<cl_float> expanded = device.expand_image(rgb, width, height);

	glEnable(GL_TEXTURE_2D);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, rgb.data());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_FLOAT, grayscale.data());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, bump_map.data());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 1024, 0, GL_RGBA, GL_FLOAT, expanded.data());
}

void onDisplay() {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, 2);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 1); glVertex2f(-1.0f, 1.0f);
	glTexCoord2i(1, 1); glVertex2f(1.0f, 1.0f);
	glTexCoord2i(1, 0); glVertex2f(1.0f, -1.0f);
	glTexCoord2i(0, 0); glVertex2f(-1.0f, -1.0f);
	glEnd();

	console.render_console();

	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
	int modifier = glutGetModifiers();
	if (modifier & GLUT_ACTIVE_SHIFT) {
		console.shift_pressed();
	} else {
		console.shift_released();
	}
	switch (key) {
		case GLUT_KEY_ESC:
			glutLeaveFullScreen();
			glutDestroyWindow(1);
			exit(0);
			break;
		default:
			if (console.is_open()) {
				console.on_keyboard(key);
			} else {
				if (key == ' ')
					glutFullScreenToggle();
				keys_down[key] = true;
			}
			break;
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
	switch (key) {
		case GLUT_KEY_F1:
			console.toggle_console();
			break;
		default:
			if (console.is_open()) {
				console.on_special(key);
			}
			break;
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

float last_time = 0;
float time_elapsed = 0;
int frames = 0;
float wait_time = 0;
float acc = 0;
void onIdle() {
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float dt = time - last_time;
	last_time = time;
	time_elapsed = time_elapsed + dt;

	if (frames >= 1) {
		wait_time = std::max((1.0f - (time_elapsed * max_fps / 1 - max_fps * wait_time)) / max_fps, 0.0f);
		frames = 0;
		time_elapsed = 0;
	}

	acc = acc + dt;
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
