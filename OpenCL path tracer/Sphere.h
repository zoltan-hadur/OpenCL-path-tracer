#pragma once

#include <format>
#include <CL\opencl.hpp>
#include "GLConsole.h"
#include "ConsoleVariable.h"
#include "Vector3.h"

#pragma pack(1)
__declspec(align(16))
class Sphere {
public:
	Vector3 pos;		// Coordinate of the center
	Vector3 ori;		// Orientation
	cl_float r;		// Radius
	cl_uint mati;	// Index to material
	cl_uint texi;	// Index to texture info
	void console_add();
public:
	Sphere(Vector3 pos, Vector3 ori, cl_float r, cl_uint mati, cl_uint texi = 0);
};
#pragma pack()

Sphere::Sphere(Vector3 pos, Vector3 ori, cl_float r, cl_uint mati, cl_uint texi) {
	this->pos = pos;
	this->ori = ori;
	this->r = r;
	this->mati = mati;
	this->texi = texi;
	this->console_add();
}

void Sphere::console_add() {
	static unsigned int id = 0;
	GLConsole::cvars.Add(ConsoleVariable(&pos, std::format("app.objects.spheres[{}].pos", id)));
    GLConsole::cvars.Add(ConsoleVariable(&ori, std::format("app.objects.spheres[{}].ori", id)));
    GLConsole::cvars.Add(ConsoleVariable(&r, std::format("app.objects.spheres[{}].r", id)));
    GLConsole::cvars.Add(ConsoleVariable(&mati, std::format("app.objects.spheres[{}].mati", id)));
    GLConsole::cvars.Add(ConsoleVariable(&texi, std::format("app.objects.spheres[{}].texi", id)));
	id++;
}