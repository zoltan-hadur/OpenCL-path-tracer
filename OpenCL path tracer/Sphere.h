#pragma once

#include <CL\opencl.hpp>
#include "GLConsole.h"
#include "Vector3.h"

class Sphere {
public:
	Float3 pos;		// Coordinate of the center
	Float3 ori;		// Orientation
	cl_float r;		// Radius
	cl_uint mati;	// Index to material
	cl_uint texi;	// Index to texture info
	cl_uint reserved;
	void console_add();
public:
	Sphere(Float3 pos, Float3 ori, cl_float r, cl_uint mati, cl_uint texi = 0);
};

Sphere::Sphere(Float3 pos, Float3 ori, cl_float r, cl_uint mati, cl_uint texi) {
	this->pos = pos;
	this->ori = ori;
	this->r = r;
	this->mati = mati;
	this->texi = texi;
	this->console_add();
}

void Sphere::console_add() {
	static unsigned int id = 0;
	GLConsole::cvars.attach_cvar("app.objects.spheres[" + std::to_string(id) + "].pos", &pos);
	GLConsole::cvars.attach_cvar("app.objects.spheres[" + std::to_string(id) + "].ori", &ori);
	GLConsole::cvars.attach_cvar("app.objects.spheres[" + std::to_string(id) + "].r", &r);
	GLConsole::cvars.attach_cvar("app.objects.spheres[" + std::to_string(id) + "].mati", &mati);
	GLConsole::cvars.attach_cvar("app.objects.spheres[" + std::to_string(id) + "].texi", &texi);
	id++;
}