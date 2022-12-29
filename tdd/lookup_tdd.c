#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "lookup_tdd.h"
#include "lookup.h"
#include "board_constants.h"
#include "bitboards.h"

static bool OneSetBit(Bitboard_t b) {return b & (b - 1);}

static void ShouldInitializeSingleBitset() {
    for(int i = 0; i < 64; i++) {
        Bitboard_t bitset = GetSingleBitset(i);
        if(OneSetBit(bitset)) {
            printf("\n%s Failure\n", __func__);
            return;
        }
    }

    printf(".");
}

void LookupTDDRunner() {
    InitLookup();

    ShouldInitializeSingleBitset();
}