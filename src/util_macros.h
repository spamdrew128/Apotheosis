#ifndef __UTIL_MACROS_H__
#define __UTIL_MACROS_H__

#define NUM_ARRAY_ELEMENTS(array) sizeof(array) / sizeof(*array)

#define CLAMP_TO_RANGE(n, min, max) \
    do { \
        if(n < min) { \
            n = min; \
        } else if(n > max) { \
            n = max; \
        } \
    } while(0)

#define MIN(v1, v2) ((v1) < (v2)) ? (v1) : (v2)

#define MAX(v1, v2) ((v1) > (v2)) ? (v1) : (v2)

#define MIRROR(sq) ((sq)^56)

#endif
