#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Material {
private:
	float3 kd;				// Diffuse color
	float3 ks;				// Specular color
	float3 emission;		// Emission
	cl_float n;				// Refractive index
	cl_float glossiness;	// Glossiness
	cl_uint type;			// 0-emitter, 1-diffuse or specular, 2-refractive
	cl_uint reserved;
public:
	Material(float3 kd, float3 ks, float3 emission, cl_float n, cl_float glossiness, cl_uint type);
};

Material::Material(float3 kd, float3 ks, float3 emission, cl_float n, cl_float glossiness, cl_uint type) {
	this->kd = kd;
	this->ks = ks;
	this->emission = emission;
	this->n = n;
	this->glossiness = glossiness;
	this->type = type;
}