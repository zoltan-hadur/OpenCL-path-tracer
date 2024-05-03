#pragma once

#include <cstdint>

#include "TextureType.h"

namespace OpenCL_PathTracer
{
#pragma pack(1)
    __declspec(align(4))
    class TextureInfo
    {
    private:
        float _textureWidth;
        float _textureHeight;
        uint32_t _textureIndex;
        TextureType _textureType;
    public:
        TextureInfo(float textureWidth, float textureHeight, uint32_t textureIndex, TextureType textureType);
    };
#pragma pack()
}