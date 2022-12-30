#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include "board_constants.h"

typedef struct {
    Bitboard_t singleBitsets[NUM_SQUARES];

    Bitboard_t knightAttacks[NUM_SQUARES];
    Bitboard_t kingAttacks[NUM_SQUARES];
} Lookup_t;

void InitLookup();

Bitboard_t GetSingleBitset(Square_t square);

Bitboard_t GetKnightAttacks(Square_t square);

Bitboard_t GetKingAttacks(Square_t square);

#endif