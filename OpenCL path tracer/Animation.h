#pragma once

#include "EasingFunction.h"

#include <functional>

namespace OpenCL_PathTracer
{
    namespace GL_Stuff
    {
        template<typename T>
        class Animation
        {
        private:
            EasingFunction _easingFunction;
            bool _isRunning;
            bool _isReversed;
            float _elapsed;
            std::function<void(Animation&)> _onFinishedcallback;

        public:
            T From;
            T To;
            float Duration;

            Animation() : Animation(T(), T())
            {

            }

            Animation(T from, T to, float duration = 0.333f, EasingFunction easingFunction = EasingFunction())
            {
                From = std::move(from);
                To = std::move(to);
                Duration = duration;
                _easingFunction = std::move(easingFunction);
                _isRunning = false;
                _isReversed = false;
                _elapsed = 0;
                _onFinishedcallback = nullptr;
            }

            EasingFunction const& GetEasingFunction()
            {
                return _easingFunction;
            }

            bool IsRunning() const
            {
                return _isRunning;
            }

            T GetCurrentValue() const
            {
                if (_elapsed == 0)
                {
                    return _isReversed ? To : From;
                }
                else if (_elapsed == Duration)
                {
                    return _isReversed ? From : To;
                }
                else
                {
                    return _isReversed ?
                        To + (From - To) * _easingFunction.GetValue(_elapsed / Duration) :
                        From + (To - From) * _easingFunction.GetValue(_elapsed / Duration);
                }
            }

            void SetOnFinishedCallback(std::function<void(Animation&)> callback)
            {
                _onFinishedcallback = callback;
            }

            void Start()
            {
                _elapsed = 0;
                _isRunning = true;
            }
            
            void Animate(float dt)
            {
                if (_isRunning)
                {
                    _elapsed = _elapsed + dt;
                    if (_elapsed >= Duration)
                    {
                        _isRunning = false;
                        if (_onFinishedcallback)
                        {
                            _onFinishedcallback(*this);
                        }
                    }
                }
            }

            void Reverse()
            {
                _elapsed = 0;
                _isReversed = !_isReversed;
            }
        };
    }
}