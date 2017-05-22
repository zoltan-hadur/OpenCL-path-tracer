#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Material {
private:
	float3 kd, ks, emission;				// Diffuse, specular, emission
	cl_float R0, n, shininess, glossiness;	// Fresnel (Schlick's approximation), refractive index, shininess
	cl_char refractive;
public:
	Material(float3 kd, float3 ks, float3 emission, cl_float n, cl_float shininess, cl_char refractive);
};

Material::Material(float3 kd, float3 ks, float3 emission, cl_float n, cl_float shininess, cl_char refractive) {
	this->kd = kd;
	this->ks = ks;
	this->emission = emission;
	this->n = n;
	this->shininess = shininess;
	this->refractive = refractive;
	this->R0 = ((1 - n) / (1 + n)) * ((1 - n) / (1 + n));
}