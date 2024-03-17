#pragma once

#include <CL\opencl.hpp>
#include "Vector3.h"

#pragma pack(1)
__declspec(align(16))
class Triangle {
public:
	Vector3 v1;			// Vertices
	Vector3 v2;			//
	Vector3 v3;			// 
	cl_uint mati;		// Index to the material
	cl_uint texi;		// Index to the texture info
public:
	Triangle(Vector3 v1, Vector3 v2, Vector3 v3, cl_uint mati, cl_uint texi = 0);
};
#pragma pack()

Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3, cl_uint mati, cl_uint texi) {
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->mati = mati;
	this->texi = texi;
}