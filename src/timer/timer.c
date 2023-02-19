#include "timer.h"

static clock_t MillisecondsToClockClocks(Milliseconds_t ms) {
    // idk if CPS is the same across all systems so this is a precaution
    double clocksPerMs = ((1 / (double)msec_per_sec) * CLOCKS_PER_SEC); 
    return ms * clocksPerMs;
}

void TimerInit(Timer_t* timer, Milliseconds_t duration) {
    timer->startTime = clock();
    timer->endTime = timer->startTime + MillisecondsToClockClocks(duration);
}

bool TimerExpired(Timer_t* timer) {
    return clock() > timer->endTime;
}