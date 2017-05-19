#pragma once

#include <GL\freeglut.h>

class Stopwatch {
private:
	float last_time;
public:
	Stopwatch();
	float get_delta_time();
};

Stopwatch::Stopwatch() {
	last_time = 0;
}

float Stopwatch::get_delta_time() {
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float dt = time - last_time;
	last_time = time;
	return dt;
}