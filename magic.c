#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "magic.h"
#include "magic_table.h"
#include "bitboards.h"
#include "RNG.h"

enum {
    uninitialized = 0xffffffffffffffff
};

typedef Bitboard_t (*BlockersToAttacksCallback_t)(Square_t, Bitboard_t);

typedef Bitboard_t (*FindMaskCallback_t)(Square_t);

typedef struct {
    Bitboard_t blockers;
    Bitboard_t attacks;
} TempStorage_t;

static uint32_t spaceUsed = 0;

// To avoid lookup dependancies
static Bitboard_t SquareToBitset(Square_t square) {
    return C64(1) << square;
}

static int DistinctBlockers(int n) {
    return (int)(C64(1) << n);
}

static void InitHashTable(Bitboard_t* hashTable, int tableEntries) {
    for(int i = 0; i < tableEntries; i++) {
        hashTable[i] = uninitialized;
    }
}

static Bitboard_t FillMask(Bitboard_t singleBitset, DirectionCallback_t DirectionFunc) {
    Bitboard_t result = empty_set;

    singleBitset = DirectionFunc(singleBitset);
    while(DirectionFunc(singleBitset)) {
        SetBits(&result, singleBitset);
        singleBitset = DirectionFunc(singleBitset);
    }

    return result;
}

static Bitboard_t FillAttacks(Bitboard_t singleBitset, Bitboard_t blockers, DirectionCallback_t DirectionFunc) {
    Bitboard_t result = empty_set;

    do {
        singleBitset = DirectionFunc(singleBitset);
        SetBits(&result, singleBitset);
    } while(singleBitset && !(singleBitset & blockers));

    return result;
}

static Bitboard_t* CreateHashTable(uint8_t indexBits) {
    int tableEntries = DistinctBlockers(indexBits);
    Bitboard_t* hashTable = malloc(tableEntries * sizeof(*hashTable));

    InitHashTable(hashTable, tableEntries);

    return hashTable;
}

static Bitboard_t FindRookMask(Square_t square) {
    Bitboard_t singleBitset = SquareToBitset(square);
    return (
        FillMask(singleBitset, NortOne) |
        FillMask(singleBitset, EastOne) |
        FillMask(singleBitset, SoutOne) |
        FillMask(singleBitset, WestOne)
    );
}

static Bitboard_t FindBishopMask(Square_t square) {
    Bitboard_t singleBitset = SquareToBitset(square);
    return (
        FillMask(singleBitset, NoEaOne) |
        FillMask(singleBitset, SoEaOne) |
        FillMask(singleBitset, SoWeOne) |
        FillMask(singleBitset, NoWeOne)
    );
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

static Bitboard_t FindBishopAttacksFromBlockers(Square_t square, Bitboard_t blockers) {
    Bitboard_t singleBitset = SquareToBitset(square);
    return (
        FillAttacks(singleBitset, blockers, NoEaOne) |
        FillAttacks(singleBitset, blockers, SoEaOne) |
        FillAttacks(singleBitset, blockers, SoWeOne) |
        FillAttacks(singleBitset, blockers, NoWeOne)
    );
}

static void InitTempStorage(
    TempStorage_t* tempStorageTable, 
    Bitboard_t mask, 
    uint8_t indexBits, 
    Square_t square, 
    BlockersToAttacksCallback_t FindPieceAttacksFromBlockers
) 
{
    int tableEntries = DistinctBlockers(indexBits);
    Bitboard_t blockers = 0;

    for(int i = 0; i < tableEntries; i++) {
        tempStorageTable[i].blockers = blockers;
        tempStorageTable[i].attacks = FindPieceAttacksFromBlockers(square, blockers);
        blockers = (blockers - mask) & mask; // this iterates through all subsets somehow
    }

    assert(blockers == 0);
}

static void FillHashTable(
    MagicBB_t magic,
    Bitboard_t* hashTable,
    uint8_t shift,
    Bitboard_t mask,
    Square_t square,
    BlockersToAttacksCallback_t callback
)
{
    assert(hashTable != NULL);
    uint8_t indexBits = 64-shift;
    int tableEntries = DistinctBlockers(indexBits);

    TempStorage_t* tempStorageTable = malloc(tableEntries * sizeof(*tempStorageTable));
    InitTempStorage(tempStorageTable, mask, indexBits, square, callback);

    for(int i = 0; i < tableEntries; i++) {
        Bitboard_t blockers = tempStorageTable[i].blockers;
        Bitboard_t attacks = tempStorageTable[i].attacks;
        Hash_t hash = MagicHash(blockers, magic, shift);

        if(hashTable[hash] == uninitialized) {
           hashTable[hash] = attacks;
        } else if(hashTable[hash] != attacks) {
            assert(2 + 2 == 5); // literally 1984
        }
    }

    free(tempStorageTable);
}

static void InitMagicEntries(
    MagicEntry_t magicEntries[NUM_SQUARES],
    MagicBB_t magicTable[NUM_SQUARES],
    Bitboard_t hashTable[NUM_HASH_ENTRIES],
    FindMaskCallback_t FindMaskCallback, 
    BlockersToAttacksCallback_t BlockersToAttacksCallback
) 
{
    for(Square_t square = 0; square < NUM_SQUARES; square++) {
        magicEntries[square].mask = FindMaskCallback(square);
        uint8_t indexBits = PopulationCount(magicEntries[square].mask);
        magicEntries[square].shift = NUM_SQUARES - indexBits;

        magicEntries[square].hashTable = CreateHashTable(indexBits);
        magicEntries[square].magic = magicTable[square];

        FillHashTable(
            magicEntries[square].magic,
            magicEntries[square].hashTable,
            magicEntries[square].shift,
            magicEntries[square].mask,
            square,
            BlockersToAttacksCallback
        );
    }
}

static void InitRookEntries(MagicEntry_t magicEntries[NUM_SQUARES], Bitboard_t hashTable[NUM_HASH_ENTRIES]) {
    MagicBB_t magicTable[NUM_SQUARES] = ROOK_MAGICS;
    InitMagicEntries(magicEntries, magicTable, hashTable, FindRookMask, FindRookAttacksFromBlockers);
}

static void InitBishopEntries(MagicEntry_t magicEntries[NUM_SQUARES], Bitboard_t hashTable[NUM_HASH_ENTRIES]) {
    MagicBB_t magicTable[NUM_SQUARES] = BISHOP_MAGICS;
    InitMagicEntries(magicEntries, magicTable, hashTable, FindBishopMask, FindBishopAttacksFromBlockers);
}

void InitAllMagicEntries(
    MagicEntry_t rookMagicEntries[NUM_SQUARES],
    MagicEntry_t bishopMagicEntries[NUM_SQUARES],
    Bitboard_t hashTable[NUM_HASH_ENTRIES]
)
{
    InitHashTable(hashTable, NUM_HASH_ENTRIES);
    InitRookEntries(rookMagicEntries, hashTable);
    InitBishopEntries(bishopMagicEntries, hashTable);
}