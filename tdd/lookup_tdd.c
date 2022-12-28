#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "lookup_tdd.h"
#include "lookup.h"
#include "board_constants.h"
#include "bitboards.h"

static void ShouldInitializeSingleBitset() {
    for(int i = 0; i < 64; i++) {
        Bitboard_t bitset = GetSingleBitset(a2);
        if(bitset & (bitset - 1)) {
            printf("\n%s Failure\n",__func__);
            return;
        }
    }

    printf(".");
}

void LookupTDDRunner() {
    InitLookup();

    ShouldInitializeSingleBitset();
}