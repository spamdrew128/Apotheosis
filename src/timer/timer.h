#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "time_constants.h"

typedef struct {
    clock_t startTime;
    clock_t endTime;
} Timer_t;

void TimerInit(Timer_t* timer, Milliseconds_t duration);

bool TimerExpired(Timer_t* timer);

#endif