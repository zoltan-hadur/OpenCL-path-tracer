#pragma once

#include "Vector3.h"

namespace OpenCL_PathTracer
{
#pragma pack(1)
    __declspec(align(16))
    class Ray
    {
    private:
        Vector3 _position;
        Vector3 _direction;
    public:
        Ray(Vector3 position, Vector3 direction);
    };
#pragma pack()
}