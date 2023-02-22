#include "timer.h"

// shamelessly stolen from https://github.com/mhouppin/stash-bot
static Milliseconds_t ClockRead() {
#if defined(_WIN32) || defined(_WIN64)
    struct timeb tp;

    ftime(&tp);
    return ((Milliseconds_t)tp.time * 1000 + tp.millitm);
#else
    struct timespec tp;

    clock_gettime(CLOCK_REALTIME, &tp);
    return ((Milliseconds_t)tp.tv_sec * 1000 + tp.tv_nsec / 1000000);
#endif
}

void TimerInit(Timer_t* timer, Milliseconds_t duration) {
    timer->startTime = ClockRead();
    timer->endTime = timer->startTime + duration;
}

bool TimerExpired(Timer_t* timer) {
    return ClockRead() > timer->endTime;
}

void StopwatchInit(Stopwatch_t* stopwatch) {
    stopwatch->startTime = ClockRead();
}

Milliseconds_t ElapsedTime(Stopwatch_t* stopwatch) {
    return ClockRead() - stopwatch->startTime;
}