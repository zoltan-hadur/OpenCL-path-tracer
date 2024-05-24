#pragma once

#include <functional>
#include <numbers>
#include <algorithm>
#include <format>
#include <map>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        enum class EasingMode
        {
            EaseIn,
            EaseOut,
            EaseInOut
        };

        class EasingFunction
        {
        private:
            EasingMode _mode;
            std::function<float(float)> _func;

        public:
            EasingFunction(EasingMode mode = EasingMode::EaseInOut, std::function<float(float)> func = [](float t) { return 1 - std::sinf((1 - t) * std::numbers::pi_v<float> / 2); });

            EasingMode GetMode() const;
            float GetValue(float t) const;
        };
    }
}

template<> struct
std::formatter<OpenCL_PathTracer::GL_Stuff::EasingMode> : std::formatter<const char*>
{
    auto format(OpenCL_PathTracer::GL_Stuff::EasingMode mode, format_context& ctx) const
    {
        static std::map<OpenCL_PathTracer::GL_Stuff::EasingMode, const char*> names =
        {
            { OpenCL_PathTracer::GL_Stuff::EasingMode::EaseIn, "EaseIn" },
            { OpenCL_PathTracer::GL_Stuff::EasingMode::EaseOut, "EaseOut" },
            { OpenCL_PathTracer::GL_Stuff::EasingMode::EaseInOut, "EaseInOut" }
        };
        return formatter<const char*>::format(names[mode], ctx);
    }
};