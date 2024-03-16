#pragma once

#include <CL\opencl.hpp>
#include <iostream>

class Float3 {
private:
    cl_float3 inner;

    Float3(cl_float3 c);
public:
    Float3(float x = 0, float y = 0, float z = 0);

    float Sum();
    float Length();
    Float3 Normalize();
    float Dot(Float3 c);
    Float3 Cross(Float3 c);
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
    /// <summary>
    /// Rotates the vector v around the unit vector k by the angle theta according to the right hand rule.<br/>
    /// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    /// </summary>
    /// <param name="v">The vector to be rotated.</param>
    /// <param name="k">The unit vector which the vector v rotates around.</param>
    /// <param name="theta">The angle (in degrees) which vector v is rotated around vector k</param>
    /// <returns></returns>
    static Float3 Rotate(Float3 v, Float3 k, float theta);
};