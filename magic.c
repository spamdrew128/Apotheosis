#include <stdio.h>
#include <stdlib.h>

#include "magic.h"
#include "bitboards.h"

// To avoid lookup dependancies
#define SquareToBitset(square) C64(1) << square

#define NotEdge(singleBitset, edge) singleBitset & ~(edge)

static Bitboard_t FillMask(Bitboard_t singleBitset, Bitboard_t (*DirectionFunc)(Bitboard_t), Bitboard_t edge) {
    Bitboard_t result = C64(0);

    singleBitset = DirectionFunc(singleBitset);
    while(NotEdge(singleBitset, edge)) {
        result |= singleBitset;
        singleBitset = DirectionFunc(singleBitset);
    }

    return result;
}

static Bitboard_t FindRookMask(Square_t square) {
    Bitboard_t singleBitset = SquareToBitset(square);
    return (
        FillMask(singleBitset, NortOne, rank_8) |
        FillMask(singleBitset, EastOne, h_file) |
        FillMask(singleBitset, SoutOne, rank_1) |
        FillMask(singleBitset, WestOne, a_file)
    );
}

void InitRookEntries(MagicEntry_t magicEntries[NUM_SQUARES]) {
    for(Square_t square = 0; square < NUM_SQUARES; square++) {
        magicEntries[square].mask = FindRookMask(square);
        uint8_t indexBits = PopulationCount(magicEntries[square].mask);
        magicEntries[square].shift = NUM_SQUARES - indexBits;
    }
}