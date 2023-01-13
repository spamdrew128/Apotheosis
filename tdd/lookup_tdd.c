#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "lookup_tdd.h"
#include "lookup.h"
#include "board_constants.h"
#include "bitboards.h"

// HELPERS
static bool OneSetBit(Bitboard_t b) {return b & (b - 1);}

// TESTS
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
    ShouldInitializeSingleBitset();
}   