#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "lookup_tdd.h"
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

static void ShouldGetSlidingCheckmaskLookup() {
    Square_t kingSquare = f3;
    Square_t bishopSquare = c6;
    Bitboard_t expectedCheckmask = CreateBitboard(3, e4,d5,c6);

    PrintResults(GetSlidingCheckmask(kingSquare, bishopSquare) == expectedCheckmask);
}

static void ShouldGetOtherSlidingCheckmaskLookup() {
    Square_t kingSquare = e5;
    Square_t rookSquare = h5;
    Bitboard_t expectedCheckmask = CreateBitboard(3, f5,g5,h5);

    PrintResults(GetSlidingCheckmask(kingSquare, rookSquare) == expectedCheckmask);
}

static void ShouldCorrectlyFindDirectionalRays() {
    Square_t square = d5;
    Bitboard_t expectedRay = CreateBitboard(4, h1,g2,f3,e4);

    PrintResults(GetDirectionalRay(square, SE) == expectedRay);
}

void LookupTDDRunner() {
    ShouldInitializeSingleBitset();
    // Just pretend there are more tests here
    ShouldGetSlidingCheckmaskLookup();
    ShouldGetOtherSlidingCheckmaskLookup();

    ShouldCorrectlyFindDirectionalRays();
}   