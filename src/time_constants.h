#ifndef __TIME_CONSTANTS_H__
#define __TIME_CONSTANTS_H__

#include <stdint.h>
#include <limits.h>

enum {
    msec_per_sec = 1000,
    MSEC_MAX = INT32_MAX
};

typedef int32_t Milliseconds_t;
typedef double Seconds_t;

#endif