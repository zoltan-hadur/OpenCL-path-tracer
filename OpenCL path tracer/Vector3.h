#pragma once

#include <CL\opencl.hpp>
#include <iostream>

/// <summary>
/// Wraps cl_float3 with some additional convenient methods.
/// </summary>
class Vector3
{
private:
    cl_float3 _vector;
public:
    Vector3(float x = 0, float y = 0, float z = 0);

    float Sum() const;
    float Length() const;
    Vector3 Normalize() const;
    float Dot(Vector3 vector) const;
    Vector3 Cross(Vector3 vector) const;
    Vector3 operator+(Vector3 vector) const;
    Vector3 operator+(float value) const;
    Vector3 operator-(Vector3 vector) const;
    Vector3 operator-(float value) const;
    Vector3 operator-() const;
    Vector3 operator*(Vector3 vector) const;
    Vector3 operator*(float value) const;
    Vector3 operator/(Vector3 vector) const;
    Vector3 operator/(float value) const;
    float& operator[](int index);
    float const& operator[](int index) const;
    friend Vector3 operator+(float value, Vector3 vector);
    friend Vector3 operator-(float value, Vector3 vector);
    friend Vector3 operator*(float value, Vector3 vector);
    friend Vector3 operator/(float value, Vector3 vector);
    friend std::ostream& operator<<(std::ostream& os, Vector3 vector);
    friend std::istream& operator>>(std::istream& is, Vector3 vector);

    /// <summary>
    /// Rotates the vector v around the unit vector k by the angle theta according to the right hand rule.<br/>
    /// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    /// </summary>
    /// <param name="v">The vector to be rotated.</param>
    /// <param name="k">The unit vector which the vector v rotates around.</param>
    /// <param name="theta">The angle (in degrees) which vector v is rotated around vector k.</param>
    /// <returns></returns>
    static Vector3 Rotate(Vector3 v, Vector3 k, float theta);
};