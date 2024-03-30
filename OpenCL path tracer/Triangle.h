#pragma once

#include "Vector3.h"

#pragma pack(1)
__declspec(align(16))
class Triangle
{
public:
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    uint32_t MaterialIndex;
    uint32_t TextureInfoIndex;
public:
    Triangle(Vector3 v1, Vector3 v2, Vector3 v3, uint32_t materialIndex, uint32_t textureInfoIndex = 0);
};
#pragma pack()