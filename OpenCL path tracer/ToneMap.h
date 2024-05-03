#pragma once

#include <cstdint>

namespace OpenCL_PathTracer
{
#pragma pack(1)
    enum class ToneMap : uint8_t
    {
        RAW,
        SRGB,
        REINHARD,
        FILMIC,
        ToneMap_COUNT
    };
#pragma pack()
}