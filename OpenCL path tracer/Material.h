#pragma once

#include <CL\opencl.hpp>
#include "Float3.h"

class Material {
private:
	Float3 kd;				// Diffuse color
	Float3 ks;				// Specular color
	Float3 emission;		// Emission
	cl_float n;				// Refractive index
	cl_float glossiness;	// Glossiness
	cl_uint type;			// 0-emitter, 1-diffuse or specular, 2-refractive
	cl_uint reserved;
public:
	Material(Float3 kd, Float3 ks, Float3 emission, cl_float n, cl_float glossiness, cl_uint type);
};

Material::Material(Float3 kd, Float3 ks, Float3 emission, cl_float n, cl_float glossiness, cl_uint type) {
	this->kd = kd;
	this->ks = ks;
	this->emission = emission;
	this->n = n;
	this->glossiness = glossiness;
	this->type = type;
}