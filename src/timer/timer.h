#ifndef __TIMER_H__
#define __TIMER_H__

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <time.h>
#include <sys/timeb.h>

#include <stdbool.h>
#include <stdint.h>

#include "time_constants.h"

typedef struct {
    Milliseconds_t startTime;
    Milliseconds_t endTime;
} Timer_t;

typedef struct {
    Milliseconds_t startTime;
} Stopwatch_t;

Milliseconds_t ClockRead();

void TimerInit(Timer_t* timer, Milliseconds_t duration);

bool TimerExpired(Timer_t* timer);

void StopwatchInit(Stopwatch_t* stopwatch);

Milliseconds_t ElapsedTime(Stopwatch_t* stopwatch);

#endif
