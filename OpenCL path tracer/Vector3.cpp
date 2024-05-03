#include "Vector3.h"

#include <numbers>
#include <format>
#include <iostream>

using namespace OpenCL_PathTracer;

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z)
{

}

float Vector3::Sum() const
{
    return x + y + z;
}

float Vector3::Length() const
{
    return sqrt(Dot(*this));
}

Vector3 Vector3::Normalize() const
{
    return *this / Length();
}

float Vector3::Dot(Vector3 vector) const
{
    return (*this * vector).Sum();
}

Vector3 Vector3::Cross(Vector3 vector) const
{
    return Vector3(
        y * vector.z - vector.y * z,
        z * vector.x - vector.z * x,
        x * vector.y - vector.x * y
    );
}

Vector3 Vector3::operator+(Vector3 vector) const
{
    return Vector3(
        x + vector.x,
        y + vector.y,
        z + vector.z
    );
}

Vector3 Vector3::operator+(float value) const
{
    return *this + Vector3(value, value, value);
}

Vector3 Vector3::operator-(Vector3 vector) const
{
    return Vector3(
        x - vector.x,
        y - vector.y,
        z - vector.z
    );
}

Vector3 Vector3::operator-(float value) const
{
    return *this - Vector3(value, value, value);
}

Vector3 Vector3::operator-() const
{
    return *this * -1;
}

Vector3 Vector3::operator*(Vector3 vector) const
{
    return Vector3(
        x * vector.x,
        y * vector.y,
        z * vector.z
    );
}

Vector3 Vector3::operator*(float value) const
{
    return *this * Vector3(value, value, value);
}

Vector3 Vector3::operator/(Vector3 vector) const
{
    return Vector3(
        x / vector.x,
        y / vector.y,
        z / vector.z
    );
}

Vector3 Vector3::operator/(float value) const
{
    return *this / Vector3(value, value, value);
}

float& Vector3::operator[](int index)
{
    switch (index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range(std::format("Index must be 0, 1 or 2! Actual value: {}.", index));
    }
}

float const& Vector3::operator[](int index) const
{
    switch (index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range(std::format("Index must be 0, 1 or 2! Actual value: {}.", index));
    }
}

Vector3 operator+(float value, Vector3 vector)
{
    return Vector3(value, value, value) + vector;
}

Vector3 operator-(float value, Vector3 vector)
{
    return Vector3(value, value, value) - vector;
}

Vector3 operator*(float value, Vector3 vector)
{
    return Vector3(value, value, value) * vector;
}

Vector3 operator/(float value, Vector3 vector)
{
    return Vector3(value, value, value) / vector;
}

std::ostream& operator<<(std::ostream& os, Vector3 const& vector)
{
    return os << std::format("[{}, {}, {}]", vector.x, vector.y, vector.z);
}

std::istream& operator>>(std::istream& is, Vector3& vector)
{
    std::string str(std::istreambuf_iterator<char>(is), {});
    sscanf_s(str.c_str(), "[%f,%f,%f]", &vector.x, &vector.y, &vector.z);
    return is;
}

Vector3 Vector3::Rotate(Vector3 v, Vector3 k, float theta)
{
    theta = theta / 180.0f * (float)std::numbers::pi;
    auto cosTheta = cos(theta);
    auto sinTheta = sin(theta);
    return v * cosTheta + k.Cross(v) * sinTheta + k * k.Dot(v) * (1 - cosTheta);
}