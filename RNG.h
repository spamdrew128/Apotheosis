#ifndef __RNG_H__
#define __RNG_H__

#include <stdint.h>

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng);

pcg32_random_t* GetRNGSeed();

#endif