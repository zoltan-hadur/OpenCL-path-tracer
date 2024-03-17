#include "Float3.h"

#include <numbers>
#include <format>

Float3::Float3(float x, float y, float z)
{
    _vector = cl_float3{ x, y, z };
}

Float3::Float3(cl_float3 vector)
{
    _vector = std::move(vector);
}

float Float3::Sum() const
{
    return (*this)[0] + (*this)[1] + (*this)[2];
}

float Float3::Length() const
{
    return sqrt(Dot(*this));
}

Float3 Float3::Normalize() const
{
    return *this / Length();
}

float Float3::Dot(Float3 vector) const
{
    return (*this * vector).Sum();
}

Float3 Float3::Cross(Float3 vector) const
{
    return Float3(
        (*this)[1] * vector[2] - vector[1] * (*this)[2],
        (*this)[2] * vector[0] - vector[2] * (*this)[0],
        (*this)[0] * vector[1] - vector[0] * (*this)[1]
    );
}

Float3 Float3::operator+(Float3 vector) const
{
    return Float3(
        (*this)[0] + vector[0],
        (*this)[1] + vector[1],
        (*this)[2] + vector[2]
    );
}

Float3 Float3::operator+(float value) const
{
    return *this + Float3(value, value, value);
}

Float3 Float3::operator-(Float3 vector) const
{
    return Float3(
        (*this)[0] - vector[0],
        (*this)[1] - vector[1],
        (*this)[2] - vector[2]
    );
}

Float3 Float3::operator-(float value) const
{
    return *this - Float3(value, value, value);
}

Float3 Float3::operator-() const
{
    return *this * -1;
}

Float3 Float3::operator*(Float3 vector) const
{
    return Float3(
        (*this)[0] * vector[0],
        (*this)[1] * vector[1],
        (*this)[2] * vector[2]
    );
}

Float3 Float3::operator*(float value) const
{
    return *this * Float3(value, value, value);
}

Float3 Float3::operator/(Float3 vector) const
{
    return Float3(
        (*this)[0] / vector[0],
        (*this)[1] / vector[1],
        (*this)[2] / vector[2]
    );
}

Float3 Float3::operator/(float value) const
{
    return *this / Float3(value, value, value);
}

float& Float3::operator[](int index)
{
    return _vector.s[index];
}

float const& Float3::operator[](int index) const
{
    return _vector.s[index];
}

Float3 operator+(float value, Float3 vector)
{
    return Float3(value, value, value) + vector;
}

Float3 operator-(float value, Float3 vector)
{
    return Float3(value, value, value) - vector;
}

Float3 operator*(float value, Float3 vector)
{
    return Float3(value, value, value) * vector;
}

Float3 operator/(float value, Float3 vector)
{
    return Float3(value, value, value) / vector;
}

std::ostream& operator<<(std::ostream& os, Float3 vector)
{
    return os << std::format("[{}, {}, {}]", vector[0], vector[1], vector[2]);
}

std::istream& operator>>(std::istream& is, Float3 vector)
{
    std::string str(std::istreambuf_iterator<char>(is), {});
    sscanf(str.c_str(), "[%f,%f,%f]", &vector[0], &vector[1], &vector[2]);
    return is;
}

Float3 Float3::Rotate(Float3 v, Float3 k, float theta)
{
    theta = theta / 180.0f * std::numbers::pi;
    auto cosTheta = cos(theta);
    auto sinTheta = sin(theta);
    return v * cosTheta + k.Cross(v) * sinTheta + k * k.Dot(v) * (1 - cosTheta);
}