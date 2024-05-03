#include "Ray.h"

using namespace OpenCL_PathTracer;

Ray::Ray(Vector3 position, Vector3 direction)
{
    _position = position;
    _direction = direction;
}