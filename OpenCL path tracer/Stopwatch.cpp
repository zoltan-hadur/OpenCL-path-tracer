#include "Stopwatch.h"

#include <stdexcept>

namespace OpenCL_PathTracer
{
    Stopwatch::Stopwatch()
    {
        _startTime = std::chrono::steady_clock::time_point::min();
        _lastTime = std::chrono::steady_clock::time_point::min();
        _timeToLive = 0;
        _isRunning = false;
    }

    bool Stopwatch::IsRunning()
    {
        return _isRunning;
    }

    float Stopwatch::GetDeltaTime()
    {
        if (!_isRunning)
        {
            throw std::runtime_error("Stopwatch is not running!");
        }

        auto time = std::chrono::steady_clock::now();
        auto dt = std::chrono::duration<float>(time - _lastTime);
        _lastTime = time;
        return dt.count();
    }

    float Stopwatch::GetElapsedTime()
    {
        return std::chrono::duration<float>((_isRunning ? std::chrono::steady_clock::now() : _lastTime) - _startTime).count();
    }

    void Stopwatch::Start(float timeToLive)
    {
        if (_isRunning)
        {
            throw std::runtime_error("Stopwatch is already running!");
        }

        _startTime = std::chrono::steady_clock::now();
        _lastTime = _startTime;
        _timeToLive = timeToLive;
        _isRunning = true;
    }

    void Stopwatch::Stop()
    {
        if (!_isRunning)
        {
            throw std::runtime_error("Stopwatch is already stopped!");
        }

        _lastTime = std::chrono::steady_clock::now();
        _isRunning = false;
    }

    bool Stopwatch::TryStop()
    {
        if (_isRunning && GetElapsedTime() > _timeToLive)
        {
            Stop();
            return true;
        }
        else
        {
            return false;
        }
    }
}