#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "debug.h"
#include "lookup_tdd.h"
#include "lookup.h"
#include "board_constants.h"
#include "bitboards.h"

static bool OneSetBit(Bitboard_t b) {return b & (b - 1);}

static void ShouldInitializeSingleBitset() {
    bool success = true;

    for(int i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t bitset = GetSingleBitset(i);
        if(OneSetBit(bitset)) {
            success = false;
        }
    }

    PrintResults(success);
}

void LookupTDDRunner() {
    InitLookup();

    ShouldInitializeSingleBitset();
}