#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

#include "time_constants.h"

typedef struct {
    Seconds_t startTime;
    Seconds_t duration;
} Timer_t;

#endif
