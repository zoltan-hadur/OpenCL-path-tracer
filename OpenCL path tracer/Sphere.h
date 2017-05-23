#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Sphere {
public:
	float3 pos;		// Coordinate of the center
	float3 ori;		// Orientation
	cl_float r;		// Radius
	cl_uint mati;	// Index to material
	cl_uint texi;	// Index to texture info
	cl_uint reserved;
public:
	Sphere(float3 pos, float3 ori, cl_float r, cl_uint mati, cl_uint texi = 0);
};

Sphere::Sphere(float3 pos, float3 ori, cl_float r, cl_uint mati, cl_uint texi) {
	this->pos = pos;
	this->ori = ori;
	this->r = r;
	this->mati = mati;
	this->texi = texi;
}