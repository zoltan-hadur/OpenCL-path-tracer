#pragma once

#include <CL\cl.hpp>
#include "float3.h"

class Material {
private:
	float3 kd, ks, emission, R0, n;		// Diffuse, specular, emission, Fresnel (Schlick's approximation), refractive index
	cl_float shininess;					// shininess
	cl_char refractive;
public:
	Material(float3 kd, float3 ks, float3 emission, float3 n, cl_float shininess, cl_char refractive);
};

Material::Material(float3 kd, float3 ks, float3 emission, float3 n, cl_float shininess, cl_char refractive) {
	this->kd = kd;
	this->ks = ks;
	this->emission = emission;
	this->n = n;
	this->shininess = shininess;
	this->refractive = refractive;
	this->R0 = ((1 - n) / (1 + n)) * ((1 - n) / (1 + n));
}