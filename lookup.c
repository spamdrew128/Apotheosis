#include "lookup.h"

static Lookup_t lookup;

static void InitSingleBitset(Bitboard_t singleBitsets[]) {
    for(int i = 0; i < 64; i++) {
        singleBitsets[i] = C64(1) << i;
    }
}

void InitLookup() {
    InitSingleBitset(lookup.singleBitsets);
}

Bitboard_t GetSingleBitset(Square_t square) {
    return lookup.singleBitsets[square];
}