#pragma once

#include <chrono>

namespace OpenCL_PathTracer
{
    class Stopwatch
    {
    private:
        std::chrono::steady_clock::time_point _startTime;
        std::chrono::steady_clock::time_point _lastTime;
        float _timeToLive;
        bool _isRunning;

    public:
        Stopwatch();

        bool IsRunning();
        float GetDeltaTime();
        float GetElapsedTime();

        void Start(float timeToLive = 0);
        void Stop();
        bool TryStop();
    };
}