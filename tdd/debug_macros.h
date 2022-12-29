#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__

#include <stdio.h>

#define PrintResults(success) \
    if(success) {printf(".");} else {printf("\n%s Failure\n", __func__);}

#endif