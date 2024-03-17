#pragma once

#include "Vector3.h"

#pragma pack(1)
__declspec(align(16))
class Ray {
private:
	Vector3 pos;		// Origin of the ray
	Vector3 dir;		// Direction of the ray
public:
	Ray(Vector3 o = Vector3(), Vector3 d = Vector3(0, 0, 1));
};
#pragma pack()

Ray::Ray(Vector3 o, Vector3 d) {
	pos = o;
	dir = d;
}