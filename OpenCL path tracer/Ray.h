#pragma once

#include "float3.h"

class Ray {
private:
	float3 pos;		// Origin of the ray
	float3 dir;		// Direction of the ray
public:
	Ray(float3 o = float3(), float3 d = float3(0, 0, 1));
};

Ray::Ray(float3 o, float3 d) {
	pos = o;
	dir = d;
}