#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Sphere {
public:
	float3 pos, orientation;	// Coordinate of the center, orientation
	cl_float r;					// Radius
	cl_short mati, texi;		// Index to material, index to texture info
public:
	Sphere(float3 pos, float3 orientation, cl_float r, cl_short mati, cl_short texi = 0);
};

Sphere::Sphere(float3 pos, float3 orientation, cl_float r, cl_short mati, cl_short texi) {
	this->pos = pos;
	this->orientation = orientation;
	this->r = r;
	this->mati = mati;
	this->texi = texi;
}