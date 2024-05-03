#include "TextureInfo.h"

namespace OpenCL_PathTracer
{
    TextureInfo::TextureInfo(float textureWidth, float textureHeight, uint32_t textureIndex, TextureType textureType)
    {
        _textureWidth = textureWidth;
        _textureHeight = textureHeight;
        _textureIndex = textureIndex;
        _textureType = textureType;
    }
}