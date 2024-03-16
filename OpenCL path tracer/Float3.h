#pragma once

#include <CL\opencl.hpp>
#include <ostream>

class Float3 {
private:
	cl_float3 inner;
public:
	Float3(float x = 0, float y = 0, float z = 0);
	Float3(cl_float3 c);

	Float3 sRGB2linear();
	Float3 linear2sRGB();
	float sum();
	float length();
	Float3 normalize();
	float dot(Float3 c);
	Float3 cross(Float3 c);
	Float3 operator+(Float3 c);
	Float3 operator+(float c);
	Float3 operator-(Float3 c);
	Float3 operator-(float c);
	Float3 operator*(Float3 c);
	Float3 operator*(float c);
	Float3 operator/(Float3 c);
	Float3 operator/(float c);
	float& operator[](int i);
	friend Float3 operator+(float c, Float3 r);
	friend Float3 operator-(float c, Float3 r);
	friend Float3 operator*(float c, Float3 r);
	friend Float3 operator/(float c, Float3 r);
	friend std::ostream& operator<<(std::ostream& os, Float3& c);
	friend std::istream& operator>>(std::istream& is, Float3& c);
	static Float3 rodrigues(Float3 v, Float3 k, float theta);
};

Float3::Float3(float x, float y, float z) {
	inner = cl_float3{ x, y, z };
}

Float3::Float3(cl_float3 c) {
	inner = c;
}

Float3 Float3::sRGB2linear() {
	Float3 inner = Float3(this->inner);
	for (int i = 0; i < 3; ++i) {
		if (inner[i] < 0.04045f)
			inner[i] = inner[i] / 12.92f;
		else
			inner[i] = pow((inner[i] + 0.055f) / 1.055f, 2.4f);
	}
	return inner;
}

Float3 Float3::linear2sRGB() {
	Float3 inner = Float3(this->inner);
	for (int i = 0; i < 3; ++i) {
		if (inner[i] <= 0.0031308f)
			inner[i] = inner[i] * 12.92f;
		else
			inner[i] = pow(inner[i], 1.0f / 2.4f) * 1.055f - 0.055f;
	}
	return inner;
}

float Float3::sum() {
	Float3 inner = Float3(this->inner);
	return inner[0] + inner[1] + inner[2];
}

float Float3::length() {
	Float3 inner = Float3(this->inner);
	return sqrt((inner * inner).sum());
}

Float3 Float3::normalize() {
	Float3 inner = Float3(this->inner);
	return inner / inner.length();
}

float Float3::dot(Float3 c) {
	Float3 inner = Float3(this->inner);
	return (inner * c).sum();
}

Float3 Float3::cross(Float3 c) {
	Float3 inner = Float3(this->inner);
	return Float3(inner[1] * c[2] - c[1] * inner[2],
				  inner[2] * c[0] - c[2] * inner[0],
				  inner[0] * c[1] - c[0] * inner[1]);
}

Float3 Float3::operator+(Float3 c) {
	Float3 inner = Float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] + c[i];
	return inner;
}

Float3 Float3::operator+(float c) {
	Float3 inner = Float3(this->inner);
	return inner + Float3(c, c, c);
}

Float3 Float3::operator-(Float3 c) {
	Float3 inner = Float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] - c[i];
	return inner;
}

Float3 Float3::operator-(float c) {
	Float3 inner = Float3(this->inner);
	return inner - Float3(c, c, c);
}

Float3 Float3::operator*(Float3 c) {
	Float3 inner = Float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] * c[i];
	return inner;
}

Float3 Float3::operator*(float c) {
	Float3 inner = Float3(this->inner);
	return inner * Float3(c, c, c);
}

Float3 Float3::operator/(Float3 c) {
	Float3 inner = Float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] / c[i];
	return inner;
}

Float3 Float3::operator/(float c) {
	Float3 inner = Float3(this->inner);
	return inner / Float3(c, c, c);
}

float& Float3::operator[](int i) {
	switch (i) {
		case 0:
			return inner.x;
			break;
		case 1:
			return inner.y;
			break;
		default:
			return inner.z;
			break;
	}
}

Float3 operator+(float c, Float3 r) {
	return Float3(c, c, c) + r;
}

Float3 operator-(float c, Float3 r) {
	return Float3(c, c, c) - r;
}

Float3 operator*(float c, Float3 r) {
	return Float3(c, c, c) * r;
}

Float3 operator/(float c, Float3 r) {
	return Float3(c, c, c) / r;
}

std::ostream & operator<<(std::ostream & os, Float3& c) {
	os << "[" << c[0] << ", " << c[1] << ", " << c[2] << "]";
	return os;
}

std::istream & operator>>(std::istream & is, Float3& c) {
	//is >> c[0] >> c[1] >> c[2];
	std::string str(std::istreambuf_iterator<char>(is), {});
	sscanf(str.c_str(),"[%f,%f,%f]", &c[0], &c[1], &c[2]);
	return is;
}

Float3 Float3::rodrigues(Float3 v, Float3 k, float theta) {
	theta = theta / 180.0f * 3.14159265358979323846f;
	float cost = cos(theta);
	float sint = sin(theta);
	return v*cost + k.cross(v)*sint + k*k.dot(v)*(1 - cost);
}