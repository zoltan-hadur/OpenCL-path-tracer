#pragma once

#include <GL\freeglut.h>

class Stopwatch
{
private:
    float start_time;
    float last_time;
    float TTL;
    bool running;
public:
    Stopwatch();
    void start(float time_to_live = 0);
    void stop();
    bool try_stop();
    bool is_running();
    float get_delta_time();
    float get_elapsed_time();
};