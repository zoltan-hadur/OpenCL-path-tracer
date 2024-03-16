#pragma once

#include "Float3.h"

class Ray {
private:
	Float3 pos;		// Origin of the ray
	Float3 dir;		// Direction of the ray
public:
	Ray(Float3 o = Float3(), Float3 d = Float3(0, 0, 1));
};

Ray::Ray(Float3 o, Float3 d) {
	pos = o;
	dir = d;
}