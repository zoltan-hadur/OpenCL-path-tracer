#pragma once

#include "float3.h"

class Ray {
private:
	float3 origin, direction;
public:
	Ray(float3 o = float3(), float3 d = float3(0, 0, 1));
	friend std::ostream& operator<<(std::ostream& os, Ray& ray);
};

Ray::Ray(float3 o, float3 d) {
	origin = o;
	direction = d;
}

std::ostream& operator<<(std::ostream& os, Ray& ray) {
	os << "origin = " << ray.origin << " direction = " << ray.direction;
	return os;
}