#pragma once

#include <CL\opencl.hpp>
#include "Vector3.h"

class Triangle {
public:
	Float3 v1;			// Vertices
	Float3 v2;			//
	Float3 v3;			// 
	cl_uint mati;		// Index to the material
	cl_uint texi;		// Index to the texture info
	cl_uint reserved[2];
public:
	Triangle(Float3 v1, Float3 v2, Float3 v3, cl_uint mati, cl_uint texi = 0);
};

Triangle::Triangle(Float3 v1, Float3 v2, Float3 v3, cl_uint mati, cl_uint texi) {
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->mati = mati;
	this->texi = texi;
}