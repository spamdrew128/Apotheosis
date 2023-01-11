#ifndef __RNG_H__
#define __RNG_H__

#include <stdint.h>

#include "board_constants.h"

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

Bitboard_t RandBitboard();

#endif