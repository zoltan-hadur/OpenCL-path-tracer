#include "Vector3.h"

#include <numbers>
#include <format>

Vector3::Vector3(float x, float y, float z)
{
    _vector = cl_float3{ x, y, z };
}

Vector3::Vector3(cl_float3 vector)
{
    _vector = std::move(vector);
}

float Vector3::Sum() const
{
    return (*this)[0] + (*this)[1] + (*this)[2];
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
        (*this)[1] * vector[2] - vector[1] * (*this)[2],
        (*this)[2] * vector[0] - vector[2] * (*this)[0],
        (*this)[0] * vector[1] - vector[0] * (*this)[1]
    );
}

Vector3 Vector3::operator+(Vector3 vector) const
{
    return Vector3(
        (*this)[0] + vector[0],
        (*this)[1] + vector[1],
        (*this)[2] + vector[2]
    );
}

Vector3 Vector3::operator+(float value) const
{
    return *this + Vector3(value, value, value);
}

Vector3 Vector3::operator-(Vector3 vector) const
{
    return Vector3(
        (*this)[0] - vector[0],
        (*this)[1] - vector[1],
        (*this)[2] - vector[2]
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
        (*this)[0] * vector[0],
        (*this)[1] * vector[1],
        (*this)[2] * vector[2]
    );
}

Vector3 Vector3::operator*(float value) const
{
    return *this * Vector3(value, value, value);
}

Vector3 Vector3::operator/(Vector3 vector) const
{
    return Vector3(
        (*this)[0] / vector[0],
        (*this)[1] / vector[1],
        (*this)[2] / vector[2]
    );
}

Vector3 Vector3::operator/(float value) const
{
    return *this / Vector3(value, value, value);
}

float& Vector3::operator[](int index)
{
    return _vector.s[index];
}

float const& Vector3::operator[](int index) const
{
    return _vector.s[index];
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

std::ostream& operator<<(std::ostream& os, Vector3 vector)
{
    return os << std::format("[{}, {}, {}]", vector[0], vector[1], vector[2]);
}

std::istream& operator>>(std::istream& is, Vector3 vector)
{
    std::string str(std::istreambuf_iterator<char>(is), {});
    sscanf(str.c_str(), "[%f,%f,%f]", &vector[0], &vector[1], &vector[2]);
    return is;
}

Vector3 Vector3::Rotate(Vector3 v, Vector3 k, float theta)
{
    theta = theta / 180.0f * std::numbers::pi;
    auto cosTheta = cos(theta);
    auto sinTheta = sin(theta);
    return v * cosTheta + k.Cross(v) * sinTheta + k * k.Dot(v) * (1 - cosTheta);
}