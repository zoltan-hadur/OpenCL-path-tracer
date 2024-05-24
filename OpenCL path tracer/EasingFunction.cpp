#include "EasingFunction.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        EasingFunction::EasingFunction(EasingMode mode, std::function<float(float)> func) : _mode(mode), _func(std::move(func))
        {
            
        }

        EasingMode EasingFunction::GetMode() const
        {
            return _mode;
        }

        float EasingFunction::GetValue(float t) const
        {
            switch (_mode)
            {
                case EasingMode::EaseIn:
                    return _func(t);
                case EasingMode::EaseOut:
                    return 1 - _func(1 - t);
                case EasingMode::EaseInOut:
                    return t < 0.5f ?
                        _func(2.0f * t) / 2.0f :
                        0.5f + (1.0f - _func(2.0f * (1.0f - t))) / 2.0f;
                default:
                    throw std::runtime_error(std::format("Enum value '{0}' is not supported!", _mode));
            }
        }
    }
}