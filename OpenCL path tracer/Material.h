#pragma once

#include <CL\opencl.hpp>
#include "Vector3.h"

#pragma pack(1)
__declspec(align(16))
class Material {
private:
	Vector3 kd;				// Diffuse color
	Vector3 ks;				// Specular color
	Vector3 emission;		// Emission
	cl_float n;				// Refractive index
	cl_float glossiness;	// Glossiness
	cl_uint type;			// 0-emitter, 1-diffuse or specular, 2-refractive
public:
	Material(Vector3 kd, Vector3 ks, Vector3 emission, cl_float n, cl_float glossiness, cl_uint type);
};
#pragma pack()

Material::Material(Vector3 kd, Vector3 ks, Vector3 emission, cl_float n, cl_float glossiness, cl_uint type) {
	this->kd = kd;
	this->ks = ks;
	this->emission = emission;
	this->n = n;
	this->glossiness = glossiness;
	this->type = type;
}