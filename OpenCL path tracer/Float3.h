#pragma once

#include <CL\opencl.hpp>
#include <iostream>

class Float3
{
private:
    cl_float3 _vector;

    Float3(cl_float3 vector);
public:
    Float3(float x = 0, float y = 0, float z = 0);

    float Sum() const;
    float Length() const;
    Float3 Normalize() const;
    float Dot(Float3 vector) const;
    Float3 Cross(Float3 vector) const;
    Float3 operator+(Float3 vector) const;
    Float3 operator+(float value) const;
    Float3 operator-(Float3 vector) const;
    Float3 operator-(float value) const;
    Float3 operator-() const;
    Float3 operator*(Float3 vector) const;
    Float3 operator*(float value) const;
    Float3 operator/(Float3 vector) const;
    Float3 operator/(float value) const;
    float& operator[](int index);
    float const& operator[](int index) const;
    friend Float3 operator+(float value, Float3 vector);
    friend Float3 operator-(float value, Float3 vector);
    friend Float3 operator*(float value, Float3 vector);
    friend Float3 operator/(float value, Float3 vector);
    friend std::ostream& operator<<(std::ostream& os, Float3 vector);
    friend std::istream& operator>>(std::istream& is, Float3 vector);

    /// <summary>
    /// Rotates the vector v around the unit vector k by the angle theta according to the right hand rule.<br/>
    /// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    /// </summary>
    /// <param name="v">The vector to be rotated.</param>
    /// <param name="k">The unit vector which the vector v rotates around.</param>
    /// <param name="theta">The angle (in degrees) which vector v is rotated around vector k.</param>
    /// <returns></returns>
    static Float3 Rotate(Float3 v, Float3 k, float theta);
};