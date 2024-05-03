#pragma once

#include <iosfwd>

namespace OpenCL_PathTracer
{
    class Vector3;

    class Vector2
    {
    public:
        float x;
        float y;

        Vector2();
        Vector2(float x, float y);

        float Sum() const;
        float Length() const;
        Vector2 Normalize() const;
        float Dot(Vector2 vector) const;
        Vector3 Cross(Vector2 vector) const;
        Vector2 operator+(Vector2 vector) const;
        Vector2 operator+(float value) const;
        Vector2 operator-(Vector2 vector) const;
        Vector2 operator-(float value) const;
        Vector2 operator-() const;
        Vector2 operator*(Vector2 vector) const;
        Vector2 operator*(float value) const;
        Vector2 operator/(Vector2 vector) const;
        Vector2 operator/(float value) const;
        float& operator[](int index);
        float const& operator[](int index) const;
        friend Vector2 operator+(float value, Vector2 vector);
        friend Vector2 operator-(float value, Vector2 vector);
        friend Vector2 operator*(float value, Vector2 vector);
        friend Vector2 operator/(float value, Vector2 vector);
        friend std::ostream& operator<<(std::ostream& os, Vector2 const& vector);
        friend std::istream& operator>>(std::istream& is, Vector2& vector);
        static Vector2 Rotate(Vector2 v, float theta);
    };
}