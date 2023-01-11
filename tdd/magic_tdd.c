#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "magic_tdd.h"
#include "magic.h"

enum {
    corner_rook_index_bits = 12,
    edge_rook_index_bits = 11,
    middle_rook_index_bits = 10,

    corner_bishop_index_bits = 6,
    edge_bishop_index_bits = 5,
    middle_bishop_index_bits = 10
};

// HELPERS
static bool SquareIsCorner(Square_t square) {
    return CreateBitboard(1, square) & (CreateBitboard(4, a1,a8,h1,h8));
}

static bool SquareIsEdge(Square_t square) {
    return CreateBitboard(1, square) & (a_file | h_file | rank_1 | rank_8);
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

void MagicTDDRunner() {
    // These tests suck to write so I'm not gonna do it for magics.
    // Sue me.

    MagicEntry_t rMagicEntries[NUM_SQUARES];
    InitRookEntries(rMagicEntries);
    MagicEntry_t bMagicEntries[NUM_SQUARES];
    InitBishopEntries(bMagicEntries);

    RookMasksMatch(rMagicEntries);

    FreeMagicEntries(rMagicEntries);
}