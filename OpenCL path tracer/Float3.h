#pragma once

#include <CL\opencl.hpp>
#include <ostream>

class float3 {
private:
	cl_float3 inner;
public:
	float3(float x = 0, float y = 0, float z = 0);
	float3(cl_float3 c);

	float3 sRGB2linear();
	float3 linear2sRGB();
	float sum();
	float length();
	float3 normalize();
	float dot(float3 c);
	float3 cross(float3 c);
	float3 operator+(float3 c);
	float3 operator+(float c);
	float3 operator-(float3 c);
	float3 operator-(float c);
	float3 operator*(float3 c);
	float3 operator*(float c);
	float3 operator/(float3 c);
	float3 operator/(float c);
	float& operator[](int i);
	friend float3 operator+(float c, float3 r);
	friend float3 operator-(float c, float3 r);
	friend float3 operator*(float c, float3 r);
	friend float3 operator/(float c, float3 r);
	friend std::ostream& operator<<(std::ostream& os, float3& c);
	friend std::istream& operator>>(std::istream& is, float3& c);
	static float3 rodrigues(float3 v, float3 k, float theta);
};

float3::float3(float x, float y, float z) {
	inner = cl_float3{ x, y, z };
}

float3::float3(cl_float3 c) {
	inner = c;
}

float3 float3::sRGB2linear() {
	float3 inner = float3(this->inner);
	for (int i = 0; i < 3; ++i) {
		if (inner[i] < 0.04045f)
			inner[i] = inner[i] / 12.92f;
		else
			inner[i] = pow((inner[i] + 0.055f) / 1.055f, 2.4f);
	}
	return inner;
}

float3 float3::linear2sRGB() {
	float3 inner = float3(this->inner);
	for (int i = 0; i < 3; ++i) {
		if (inner[i] <= 0.0031308f)
			inner[i] = inner[i] * 12.92f;
		else
			inner[i] = pow(inner[i], 1.0f / 2.4f) * 1.055f - 0.055f;
	}
	return inner;
}

float float3::sum() {
	float3 inner = float3(this->inner);
	return inner[0] + inner[1] + inner[2];
}

float float3::length() {
	float3 inner = float3(this->inner);
	return sqrt((inner * inner).sum());
}

float3 float3::normalize() {
	float3 inner = float3(this->inner);
	return inner / inner.length();
}

float float3::dot(float3 c) {
	float3 inner = float3(this->inner);
	return (inner * c).sum();
}

float3 float3::cross(float3 c) {
	float3 inner = float3(this->inner);
	return float3(inner[1] * c[2] - c[1] * inner[2],
				  inner[2] * c[0] - c[2] * inner[0],
				  inner[0] * c[1] - c[0] * inner[1]);
}

float3 float3::operator+(float3 c) {
	float3 inner = float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] + c[i];
	return inner;
}

float3 float3::operator+(float c) {
	float3 inner = float3(this->inner);
	return inner + float3(c, c, c);
}

float3 float3::operator-(float3 c) {
	float3 inner = float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] - c[i];
	return inner;
}

float3 float3::operator-(float c) {
	float3 inner = float3(this->inner);
	return inner - float3(c, c, c);
}

float3 float3::operator*(float3 c) {
	float3 inner = float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] * c[i];
	return inner;
}

float3 float3::operator*(float c) {
	float3 inner = float3(this->inner);
	return inner * float3(c, c, c);
}

float3 float3::operator/(float3 c) {
	float3 inner = float3(this->inner);
	for (int i = 0; i < 3; ++i)
		inner[i] = inner[i] / c[i];
	return inner;
}

float3 float3::operator/(float c) {
	float3 inner = float3(this->inner);
	return inner / float3(c, c, c);
}

float& float3::operator[](int i) {
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

float3 operator+(float c, float3 r) {
	return float3(c, c, c) + r;
}

float3 operator-(float c, float3 r) {
	return float3(c, c, c) - r;
}

float3 operator*(float c, float3 r) {
	return float3(c, c, c) * r;
}

float3 operator/(float c, float3 r) {
	return float3(c, c, c) / r;
}

std::ostream & operator<<(std::ostream & os, float3& c) {
	os << "[" << c[0] << ", " << c[1] << ", " << c[2] << "]";
	return os;
}

std::istream & operator>>(std::istream & is, float3& c) {
	//is >> c[0] >> c[1] >> c[2];
	std::string str(std::istreambuf_iterator<char>(is), {});
	sscanf(str.c_str(),"[%f,%f,%f]", &c[0], &c[1], &c[2]);
	return is;
}

float3 float3::rodrigues(float3 v, float3 k, float theta) {
	theta = theta / 180.0f * 3.14159265358979323846f;
	float cost = cos(theta);
	float sint = sin(theta);
	return v*cost + k.cross(v)*sint + k*k.dot(v)*(1 - cost);
}