#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Material {
private:
	float3 kd;				// Diffuse color
	float3 ks;				// Specular color
	float3 emission;		// Emission
	cl_float R0;			// Reflectance at 0°
	cl_float n;				// Refractive index
	cl_float shininess;		// Shininess
	cl_float glossiness;	// Glossiness
	cl_uint refractive;		// True if refractive
	cl_uint reserved[3];
public:
	Material(float3 kd, float3 ks, float3 emission, cl_float n, cl_float shininess, cl_float glossiness, cl_uint refractive);
};

Material::Material(float3 kd, float3 ks, float3 emission, cl_float n, cl_float shininess, cl_float glossiness, cl_uint refractive) {
	this->kd = kd;
	this->ks = ks;
	this->emission = emission;
	this->R0 = ((1 - n) / (1 + n)) * ((1 - n) / (1 + n));
	this->n = n;
	this->shininess = shininess;
	this->glossiness = glossiness;
	this->refractive = refractive;
}