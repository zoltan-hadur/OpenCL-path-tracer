#pragma once

namespace OpenCL_PathTracer
{
    class FlagHelper
    {
    public:
        template<typename T>
        static bool IsFlagSet(T value, T flag)
        {
            return (value & flag) == flag;
        }
    };
}