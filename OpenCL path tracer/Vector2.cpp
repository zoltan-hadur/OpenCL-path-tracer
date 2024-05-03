#include "Vector2.h"

#include "Vector3.h"

#include <numbers>
#include <format>
#include <iostream>

namespace OpenCL_PathTracer
{
    Vector2::Vector2() : Vector2(0, 0)
    {

    }

    Vector2::Vector2(float x, float y) : x(x), y(y)
    {

    }

    float Vector2::Sum() const
    {
        return x + y;
    }

    float Vector2::Length() const
    {
        return sqrt(Dot(*this));
    }

    Vector2 Vector2::Normalize() const
    {
        return *this / Length();
    }

    float Vector2::Dot(Vector2 vector) const
    {
        return (*this * vector).Sum();
    }

    Vector3 Vector2::Cross(Vector2 vector) const
    {
        return Vector3(
            0,
            0,
            x * vector.y - vector.x * y
        );
    }

    Vector2 Vector2::operator+(Vector2 vector) const
    {
        return Vector2(
            x + vector.x,
            y + vector.y
        );
    }

    Vector2 Vector2::operator+(float value) const
    {
        return *this + Vector2(value, value);
    }

    Vector2 Vector2::operator-(Vector2 vector) const
    {
        return Vector2(
            x - vector.x,
            y - vector.y
        );
    }

    Vector2 Vector2::operator-(float value) const
    {
        return *this - Vector2(value, value);
    }

    Vector2 Vector2::operator-() const
    {
        return *this * -1;
    }

    Vector2 Vector2::operator*(Vector2 vector) const
    {
        return Vector2(
            x * vector.x,
            y * vector.y
        );
    }

    Vector2 Vector2::operator*(float value) const
    {
        return *this * Vector2(value, value);
    }

    Vector2 Vector2::operator/(Vector2 vector) const
    {
        return Vector2(
            x / vector.x,
            y / vector.y
        );
    }

    Vector2 Vector2::operator/(float value) const
    {
        return *this / Vector2(value, value);
    }

    float& Vector2::operator[](int index)
    {
        switch (index)
        {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range(std::format("Index must be 0 or 1! Actual value: {}.", index));
        }
    }

    float const& Vector2::operator[](int index) const
    {
        switch (index)
        {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range(std::format("Index must be 0 or 1! Actual value: {}.", index));
        }
    }

    Vector2 operator+(float value, Vector2 vector)
    {
        return Vector2(value, value) + vector;
    }

    Vector2 operator-(float value, Vector2 vector)
    {
        return Vector2(value, value) - vector;
    }

    Vector2 operator*(float value, Vector2 vector)
    {
        return Vector2(value, value) * vector;
    }

    Vector2 operator/(float value, Vector2 vector)
    {
        return Vector2(value, value) / vector;
    }

    std::ostream& operator<<(std::ostream& os, Vector2 const& vector)
    {
        return os << std::format("[{}, {}]", vector.x, vector.y);
    }

    std::istream& operator>>(std::istream& is, Vector2& vector)
    {
        std::string str(std::istreambuf_iterator<char>(is), {});
        sscanf_s(str.c_str(), "[%f,%f]", &vector.x, &vector.y);
        return is;
    }

    Vector2 Vector2::Rotate(Vector2 v, float theta)
    {
        theta = theta / 180.0f * (float)std::numbers::pi;
        auto cosTheta = cos(theta);
        auto sinTheta = sin(theta);
        auto v1 = Vector3(v.x, v.y, 0);
        auto k1 = Vector3(0, 0, 1);
        auto result = v1 * cosTheta + k1.Cross(v1) * sinTheta + k1 * k1.Dot(v1) * (1 - cosTheta);
        return Vector2(result.x, result.y);
    }
}