#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Triangle {
public:
	float3 v1, v2, v3;		// Vertices
	cl_short mati, texi;	// Index to material, index to texture info
public:
	Triangle(float3 v1, float3 v2, float3 v3, cl_short mati, cl_short texi = 0);
};

Triangle::Triangle(float3 v1, float3 v2, float3 v3, cl_short mati, cl_short texi) {
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->mati = mati;
	this->texi = texi;
}