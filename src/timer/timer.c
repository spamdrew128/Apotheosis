#include <time.h>

#include "timer.h"

void TimerInit(Timer_t* timer, Milliseconds_t duration) {
    timer->startTime = ((Seconds_t)clock()) / CLOCKS_PER_SEC;
    timer->endTime = (((Seconds_t)duration) / msec_per_sec) + timer->startTime;
}

bool TimerExpired(Timer_t* timer) {
    return (Seconds_t)clock() > timer->endTime;
}