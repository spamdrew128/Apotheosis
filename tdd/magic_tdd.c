#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "magic_tdd.h"
#include "magic.h"

enum {
    corner_rook_index_bits = 12,
    edge_rook_index_bits = 11,
    middle_rook_index_bits = 10,

    bishop_9index_bits_area = SquareToBitset(d4) | SquareToBitset(e4) | SquareToBitset(d5) | SquareToBitset(e5),
    bishop_7index_bits_area = 0x00003C24243C0000,
    bishop_6index_bits_area = 0x8100000000000081,
    bishop_5index_bits_area = 0x7EFFC3C3C3C3FF7E
};

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

static void BishopsMasksMatch(MagicEntry_t rMagicEntries[NUM_SQUARES]) {
    bool success = true;

    for(int square = 0; square < NUM_SQUARES; square++) {
        Bitboard_t mask = rMagicEntries[square].mask;
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

void MagicTDDRunner() {
    // These tests suck to write so I'm not gonna do it for magics.
    // Sue me.

    MagicEntry_t rMagicEntries[NUM_SQUARES];
    InitRookEntries(rMagicEntries);
    MagicEntry_t bMagicEntries[NUM_SQUARES];
    InitBishopEntries(bMagicEntries);

    RookMasksMatch(rMagicEntries);
    BishopsMasksMatch(bMagicEntries);

    FreeMagicEntries(rMagicEntries);
}