#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "magic_tdd.h"

enum {
    corner_rook_index_bits = 12,
    edge_rook_index_bits = 11,
    middle_rook_index_bits = 10,

    bishop_9index_bits_area = SquareToBitset(d4) | SquareToBitset(e4) | SquareToBitset(d5) | SquareToBitset(e5),
    bishop_7index_bits_area = 0x00003C24243C0000,
    bishop_6index_bits_area = 0x8100000000000081,
    bishop_5index_bits_area = 0x7effc3c3c3c3ff7e,

    rook_square = d3,
    rook_blockers = SquareToBitset(b3) | SquareToBitset(g3) | SquareToBitset(d7) | SquareToBitset(d8),
    rook_expected_attacks = (rank_3 | d_file) & ~(SquareToBitset(a3) | SquareToBitset(h3) | SquareToBitset(d3) | SquareToBitset(d8)),

    bishop_square = f3,
    bishop_blockers = SquareToBitset(d1) | SquareToBitset(g4) | SquareToBitset(d5) | SquareToBitset(c6),
    bishop_expected_attacks = 0x0000000850005088,

    queen_square = e3,
    queen_blockers = 0x10020420001000,
    queen_expected_attacks = 0x0010101438ef3844
};

static MagicEntry_t rMagicEntries[NUM_SQUARES];
static MagicEntry_t bMagicEntries[NUM_SQUARES];
static Bitboard_t hashTable[NUM_HASH_ENTRIES];

// HELPERS
static bool SquareIsCorner(Square_t square) {
    return CreateBitboard(1, square) & (CreateBitboard(4, a1,a8,h1,h8));
}

static bool SquareIsEdge(Square_t square) {
    return CreateBitboard(1, square) & (a_file | h_file | rank_1 | rank_8);
}

static bool SquareIsInBishopArea(Square_t square, Bitboard_t area) {
    return CreateBitboard(1, square) & area;
}

// TESTS
static void RookMasksMatch(MagicEntry_t rMagicEntries[NUM_SQUARES]) {
    bool success = true;

    for(int square = 0; square < NUM_SQUARES; square++) {
        Bitboard_t mask = rMagicEntries[square].mask;
        if(SquareIsCorner(square)) {
            success = success && (PopulationCount(mask) == corner_rook_index_bits);
        } else if(SquareIsEdge(square)) {
            success = success && (PopulationCount(mask) == edge_rook_index_bits);
        } else {
            success = success && (PopulationCount(mask) == middle_rook_index_bits);
        }
    }

    PrintResults(success);
}

static void BishopsMasksMatch(MagicEntry_t bMagicEntries[NUM_SQUARES]) {
    bool success = true;

    for(int square = 0; square < NUM_SQUARES; square++) {
        Bitboard_t mask = bMagicEntries[square].mask;
        if(SquareIsInBishopArea(square, bishop_9index_bits_area)) {
            success = success && (PopulationCount(mask) == 9);
        } else if(SquareIsInBishopArea(square, bishop_7index_bits_area)) {
            success = success && (PopulationCount(mask) == 7);
        } else if(SquareIsInBishopArea(square, bishop_6index_bits_area)) {
            success = success && (PopulationCount(mask) == 6);
        } else if(SquareIsInBishopArea(square, bishop_5index_bits_area)) {
            success = success && (PopulationCount(mask) == 5);
        } else {
            success = false;
        }
    }

    PrintResults(success);
}

static void TestRookHashLookup(MagicEntry_t rMagicEntries[NUM_SQUARES]) {
    MagicEntry_t entry = rMagicEntries[rook_square];
    Bitboard_t blockers = rook_blockers & entry.mask;
    Hash_t hash = MagicHash(blockers, entry.magic, entry.shift);

    PrintResults(entry.hashTable[hash] == rook_expected_attacks);
}

static void TestBishopHashLookup(MagicEntry_t bMagicEntries[NUM_SQUARES]) {
    MagicEntry_t entry = bMagicEntries[bishop_square];
    Bitboard_t blockers = bishop_blockers & entry.mask;
    Hash_t hash = MagicHash(blockers, entry.magic, entry.shift);

    PrintResults(entry.hashTable[hash] == bishop_expected_attacks);
}

static void TestQueenHashLookup(MagicEntry_t rMagicEntries[NUM_SQUARES], MagicEntry_t bMagicEntries[NUM_SQUARES]) {
    MagicEntry_t rEntry = rMagicEntries[queen_square];
    MagicEntry_t bEntry = bMagicEntries[queen_square];

    Bitboard_t rBlockers = queen_blockers & rEntry.mask;
    Bitboard_t bBlockers = queen_blockers & bEntry.mask;

    Hash_t rHash = MagicHash(rBlockers, rEntry.magic, rEntry.shift);
    Hash_t bHash = MagicHash(bBlockers, bEntry.magic, bEntry.shift);

    Bitboard_t queen_attacks = rEntry.hashTable[rHash] | bEntry.hashTable[bHash];

    PrintResults(queen_attacks == queen_expected_attacks);
}

void MagicTDDRunner() {
    InitAllMagicEntries(
        rMagicEntries,
        bMagicEntries,
        hashTable
    );

    // These tests suck to write so I'm not gonna do it for magics.
    // Sue me.
    RookMasksMatch(rMagicEntries);
    BishopsMasksMatch(bMagicEntries);

    TestRookHashLookup(rMagicEntries);
    TestBishopHashLookup(bMagicEntries);
    TestQueenHashLookup(rMagicEntries, bMagicEntries);
}