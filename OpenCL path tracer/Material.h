#pragma once

#include <cstdint>

#include "Vector3.h"

namespace OpenCL_PathTracer
{
#pragma pack(1)
    __declspec(align(16))
    class Material
    {
    private:
        Vector3 _kd;        // Diffuse color
        Vector3 _ks;        // Specular color
        Vector3 _emission;  // Emission
        float _n;           // Refractive index
        float _glossiness;  // Glossiness
        uint32_t _type;     // 0-emitter, 1-diffuse or specular, 2-refractive
    public:
        Material(Vector3 kd, Vector3 ks, Vector3 emission, float n, float glossiness, uint32_t type);
    };
#pragma pack()
}