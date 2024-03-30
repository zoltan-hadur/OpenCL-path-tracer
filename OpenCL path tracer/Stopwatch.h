#pragma once

#include <chrono>

class Stopwatch
{
private:
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _lastTime;
    float _timeToLive;
    bool _isRunning;
public:
    Stopwatch();

    void Start(float timeToLive = 0);
    void Stop();
    bool TryStop();
    bool IsRunning();
    float GetDeltaTime();
    float GetElapsedTime();
};