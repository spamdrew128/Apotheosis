#include <time.h>

#include "timer.h"

static inline Seconds_t CurrentClockTime() {
    return ((Seconds_t)clock()) / CLOCKS_PER_SEC;
}

void TimerInit(Timer_t* timer, Milliseconds_t duration) {
    timer->startTime = CurrentClockTime();
    timer->endTime = (((Seconds_t)duration) / msec_per_sec) + timer->startTime;
}

bool TimerExpired(Timer_t* timer) {
    return CurrentClockTime() > timer->endTime;
}