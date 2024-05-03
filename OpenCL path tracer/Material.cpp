#include "Material.h"

namespace OpenCL_PathTracer
{
    Material::Material(Vector3 kd, Vector3 ks, Vector3 emission, float n, float glossiness, uint32_t type)
    {
        _kd = kd;
        _ks = ks;
        _emission = emission;
        _n = n;
        _glossiness = glossiness;
        _type = type;
    }
}