#ifndef __RNG_H__
#define __RNG_H__

#include <stdint.h>

#include "board_constants.h"

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

typedef struct {
    pcg32_random_t seed;
} Generator_t;

void InitRNG(Generator_t* rng, bool deterministic);

uint64_t RandUnsigned64(Generator_t* generator);

#endif