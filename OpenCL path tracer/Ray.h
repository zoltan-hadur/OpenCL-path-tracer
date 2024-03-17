#pragma once

#include "Vector3.h"

class Ray {
private:
	Vector3 pos;		// Origin of the ray
	Vector3 dir;		// Direction of the ray
public:
	Ray(Vector3 o = Vector3(), Vector3 d = Vector3(0, 0, 1));
};

Ray::Ray(Vector3 o, Vector3 d) {
	pos = o;
	dir = d;
}