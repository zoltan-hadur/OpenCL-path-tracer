#include "Triangle.h"

using namespace OpenCL_PathTracer;

Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3, uint32_t materialIndex, uint32_t textureInfoIndex) : v1(v1), v2(v2), v3(v3)
{
    MaterialIndex = materialIndex;
    TextureInfoIndex = textureInfoIndex;
}