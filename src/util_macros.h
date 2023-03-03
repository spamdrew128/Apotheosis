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

#define MIRROR(sq) ((sq)^56)

#endif
