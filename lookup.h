#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include "board_constants.h"
#include "magic.h"

typedef struct {
    Bitboard_t singleBitsets[NUM_SQUARES];

    Bitboard_t knightAttacks[NUM_SQUARES];
    Bitboard_t kingAttacks[NUM_SQUARES];

    MagicEntry_t rookMagicEntries[NUM_SQUARES];
    MagicEntry_t bishopMagicEntries[NUM_SQUARES];

    Bitboard_t slidingCheckmasks[NUM_SQUARES][NUM_SQUARES];
    Bitboard_t pawnCheckmasks[2][NUM_SQUARES]; // different for each color
} Lookup_t;

void InitLookup();

Bitboard_t GetSingleBitset(Square_t square);

Bitboard_t GetKnightAttacks(Square_t square);

Bitboard_t GetKingAttacks(Square_t square);

MagicEntry_t GetRookMagicEntry(Square_t square);

MagicEntry_t GetBishopMagicEntry(Square_t square);

Bitboard_t GetSlidingCheckmask(Square_t kingSquare, Square_t slidingPieceSquare);

Bitboard_t GetPawnCheckmask(Square_t kingSquare, Color_t color);

void TeardownLookup();

#endif