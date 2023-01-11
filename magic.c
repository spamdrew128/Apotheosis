#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "magic.h"
#include "bitboards.h"

enum {
    uninitialized = 0xffffffffffffffff
};

typedef struct {
    Bitboard_t blockers;
    Bitboard_t attacks;
} TempStorage_t;

// To avoid lookup dependancies
#define SquareToBitset(square) C64(1) << square

#define NotEdge(singleBitset, edge) singleBitset & ~(edge)

#define DistinctBlockers(n) (int)(C64(1) << n)

#define MagicHash(blockers, magic, shift) (blockers * magic) >> shift

static void ResetHashTable(Bitboard_t* hashTable, int tableEntries) {
    for(int i = 0; i < tableEntries; i++) {
        hashTable[i] = uninitialized;
    }
}

static Bitboard_t RandBitboard() {
  Bitboard_t r = 0;
  for (int i = 0; i < NUM_SQUARES; i++) {
    r = r*2 + rand()%2;
  }
  return r;
}

static Bitboard_t FillMask(Bitboard_t singleBitset, Bitboard_t (*DirectionFunc)(Bitboard_t), Bitboard_t edge) {
    Bitboard_t result = C64(0);

    singleBitset = DirectionFunc(singleBitset);
    while(NotEdge(singleBitset, edge)) {
        result |= singleBitset;
        singleBitset = DirectionFunc(singleBitset);
    }

    return result;
}

static Bitboard_t FillAttacks(Bitboard_t singleBitset, Bitboard_t blockers, Bitboard_t (*DirectionFunc)(Bitboard_t)) {
    Bitboard_t result = C64(0);

    do {
        singleBitset = DirectionFunc(singleBitset);
        result |= singleBitset;
    } while(singleBitset && !(singleBitset & blockers));

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

    ResetHashTable(hashTable, tableEntries);

    return hashTable;
}

static Bitboard_t FindRookAttacksFromBlockers(Square_t square, Bitboard_t blockers) {
    Bitboard_t singleBitset = SquareToBitset(square);
    return (
        FillAttacks(singleBitset, blockers, NortOne) |
        FillAttacks(singleBitset, blockers, EastOne) |
        FillAttacks(singleBitset, blockers, SoutOne) |
        FillAttacks(singleBitset, blockers, WestOne)
    );
}

static void InitTempStorage(
    TempStorage_t* tempStorageTable, 
    Bitboard_t mask, 
    uint8_t indexBits, 
    Square_t square, 
    Bitboard_t (*FindPieceAttacksFromBlockers)(Square_t, Bitboard_t)
) 
{
    int tableEntries = DistinctBlockers(indexBits);
    Bitboard_t blockers = 0;

    for(int i = 0; i < tableEntries; i++) {
        tempStorageTable[i].blockers = blockers;
        tempStorageTable[i].attacks = FindPieceAttacksFromBlockers(square, blockers);
        blockers = (blockers - mask) & mask;
    }

    assert(blockers == 0);
}

static bool TryMagic(MagicBB_t magic, Bitboard_t* hashTable, TempStorage_t* tempStorageTable, uint8_t shift, int tableEntries) {
    for(int i = 0; i < tableEntries; i++) {
        Bitboard_t blockers = tempStorageTable[i].blockers;
        Bitboard_t attacks = tempStorageTable[i].attacks;
        Hash_t hash = MagicHash(blockers, magic, shift);

        if(hashTable[hash] == uninitialized) {
           hashTable[hash] = attacks;
        } else if(hashTable[hash] != attacks) { // if non-constuctive collision
            return false;
        }
    }

    return true;
}

static MagicBB_t FindRookMagic(Bitboard_t mask, Bitboard_t* hashTable, uint8_t indexBits, Square_t square) {
    int tableEntries = DistinctBlockers(indexBits);
    TempStorage_t* tempStorageTable = malloc(tableEntries * sizeof(*tempStorageTable));
    InitTempStorage(tempStorageTable, mask, indexBits, square, FindRookAttacksFromBlockers);

    while(true) {
        MagicBB_t magic = RandBitboard() & RandBitboard() & RandBitboard();
        if(TryMagic(magic, hashTable, tempStorageTable, 64-indexBits, tableEntries)) {
            return magic;
        } else {
            ResetHashTable(hashTable, tableEntries);
        }
    }
}

void SeedRNG() {
    srand(time(NULL));
}

void InitRookEntries(MagicEntry_t magicEntries[NUM_SQUARES]) {
    for(Square_t square = 0; square < NUM_SQUARES; square++) {
        magicEntries[square].mask = FindRookMask(square);
        uint8_t indexBits = PopulationCount(magicEntries[square].mask);
        magicEntries[square].shift = NUM_SQUARES - indexBits;

        magicEntries[square].hashTable = CreateHashTable(indexBits);
        magicEntries[square].magic = FindRookMagic(magicEntries[square].mask, magicEntries[square].hashTable, indexBits, square);
    }
}