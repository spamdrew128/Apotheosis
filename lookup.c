#include "lookup.h"

static Lookup_t lookup;

static void InitSingleBitset(Bitboard_t singleBitset[]) {
    for(int i = 0; i < 64; i++) {
        singleBitset[i] = C64(1) << i;
    }
}

void InitLookup() {
    InitSingleBitset(lookup.singleBitset);
}