#pragma once

#include <cstdint>

#pragma pack(1)
enum class RayTracingMode : uint8_t
{
    RAY_TRACING,
    PATH_TRACING,
    EXPLICIT_LIGHT_SAMPLING,
    RayTracingMode_COUNT
};
#pragma pack()