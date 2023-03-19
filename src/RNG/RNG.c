#include "RNG.h"
#include "timer.h"

#define PCG32_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

static pcg32_random_t defaultRng = PCG32_INITIALIZER;

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void InitRNG(RandomNumberGenerator_t* rng, bool deterministic) {
    if(deterministic) {
        rng->seed = defaultRng;
    } else {
        rng->seed.state = ClockRead();
        rng->seed.inc = ClockRead() / 2 + 99; // idrk what to do here.
    }
}

uint64_t RandUnsigned64(RandomNumberGenerator_t* generator) {
    uint64_t r1 = pcg32_random_r(&generator->seed);
    uint64_t r2 = pcg32_random_r(&generator->seed);
    return (r1 << 32) | r2;
}