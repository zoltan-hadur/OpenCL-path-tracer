#pragma once

#include <functional>
#include <numbers>
#include <algorithm>

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
            EasingFunction(EasingMode mode = EasingMode::EaseInOut, std::function<float(float)> func = [](float t) { return 1 - std::sinf((1 - t) * std::numbers::pi / 2); });

            EasingMode GetMode() const;
            float GetValue(float t) const;
        };
    }
}