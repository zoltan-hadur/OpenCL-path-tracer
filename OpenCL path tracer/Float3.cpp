#include "Float3.h"

Float3::Float3(float x, float y, float z)
{
    inner = cl_float3{ x, y, z };
}

Float3::Float3(cl_float3 c)
{
    inner = c;
}

float Float3::Sum()
{
    Float3 inner = Float3(this->inner);
    return inner[0] + inner[1] + inner[2];
}

float Float3::Length()
{
    Float3 inner = Float3(this->inner);
    return sqrt((inner * inner).Sum());
}

Float3 Float3::Normalize()
{
    Float3 inner = Float3(this->inner);
    return inner / inner.Length();
}

float Float3::Dot(Float3 c)
{
    Float3 inner = Float3(this->inner);
    return (inner * c).Sum();
}

Float3 Float3::Cross(Float3 c)
{
    Float3 inner = Float3(this->inner);
    return Float3(inner[1] * c[2] - c[1] * inner[2],
        inner[2] * c[0] - c[2] * inner[0],
        inner[0] * c[1] - c[0] * inner[1]);
}

Float3 Float3::operator+(Float3 c)
{
    Float3 inner = Float3(this->inner);
    for (int i = 0; i < 3; ++i)
        inner[i] = inner[i] + c[i];
    return inner;
}

Float3 Float3::operator+(float c)
{
    Float3 inner = Float3(this->inner);
    return inner + Float3(c, c, c);
}

Float3 Float3::operator-(Float3 c)
{
    Float3 inner = Float3(this->inner);
    for (int i = 0; i < 3; ++i)
        inner[i] = inner[i] - c[i];
    return inner;
}

Float3 Float3::operator-(float c)
{
    Float3 inner = Float3(this->inner);
    return inner - Float3(c, c, c);
}

Float3 Float3::operator*(Float3 c)
{
    Float3 inner = Float3(this->inner);
    for (int i = 0; i < 3; ++i)
        inner[i] = inner[i] * c[i];
    return inner;
}

Float3 Float3::operator*(float c)
{
    Float3 inner = Float3(this->inner);
    return inner * Float3(c, c, c);
}

Float3 Float3::operator/(Float3 c)
{
    Float3 inner = Float3(this->inner);
    for (int i = 0; i < 3; ++i)
        inner[i] = inner[i] / c[i];
    return inner;
}

Float3 Float3::operator/(float c)
{
    Float3 inner = Float3(this->inner);
    return inner / Float3(c, c, c);
}

float& Float3::operator[](int i)
{
    switch (i)
    {
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

Float3 operator+(float c, Float3 r)
{
    return Float3(c, c, c) + r;
}

Float3 operator-(float c, Float3 r)
{
    return Float3(c, c, c) - r;
}

Float3 operator*(float c, Float3 r)
{
    return Float3(c, c, c) * r;
}

Float3 operator/(float c, Float3 r)
{
    return Float3(c, c, c) / r;
}

std::ostream& operator<<(std::ostream& os, Float3& c)
{
    os << "[" << c[0] << ", " << c[1] << ", " << c[2] << "]";
    return os;
}

std::istream& operator>>(std::istream& is, Float3& c)
{
    //is >> c[0] >> c[1] >> c[2];
    std::string str(std::istreambuf_iterator<char>(is), {});
    sscanf(str.c_str(), "[%f,%f,%f]", &c[0], &c[1], &c[2]);
    return is;
}

Float3 Float3::Rotate(Float3 v, Float3 k, float theta)
{
    theta = theta / 180.0f * 3.14159265358979323846f;
    float cost = cos(theta);
    float sint = sin(theta);
    return v * cost + k.Cross(v) * sint + k * k.Dot(v) * (1 - cost);
}