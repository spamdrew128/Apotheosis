#include <stdio.h>
#include <stdlib.h>

#include "magic.h"
#include "bitboards.h"

enum {
    uninitialized = 0xffffffffffffffff
};

// To avoid lookup dependancies
#define SquareToBitset(square) C64(1) << square

#define NotEdge(singleBitset, edge) singleBitset & ~(edge)

#define DistinctBlockers(n) (int)(C64(1) << n)

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

static Bitboard_t* CreateHashTable(uint8_t indexBits) {
    int tableEntries = DistinctBlockers(indexBits);
    Bitboard_t* hashTable = malloc(tableEntries * sizeof(*hashTable));

    for(int i = 0; i < tableEntries; i++) {
        hashTable[i] = uninitialized;
    }

    return hashTable;
}

static MagicBB_t FindMagic(Square_t square, Bitboard_t* hashTable) {

}

void InitRookEntries(MagicEntry_t magicEntries[NUM_SQUARES]) {
    for(Square_t square = 0; square < NUM_SQUARES; square++) {
        magicEntries[square].mask = FindRookMask(square);
        uint8_t indexBits = PopulationCount(magicEntries[square].mask);
        magicEntries[square].shift = NUM_SQUARES - indexBits;
        magicEntries[square].hashTable = CreateHashTable(indexBits);
        magicEntries[square].magic = FindMagic(square, magicEntries[square].hashTable);
    }
}