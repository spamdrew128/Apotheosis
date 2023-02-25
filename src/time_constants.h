#ifndef __TIME_CONSTANTS_H__
#define __TIME_CONSTANTS_H__

#include <stdint.h>
#include <limits.h>
#include <time.h>

enum {
    msec_per_sec = 1000,
    nsec_per_msec = 1000000,
    MSEC_MAX = INT32_MAX
};

typedef int64_t Milliseconds_t;
typedef int32_t Seconds_t;

#endif