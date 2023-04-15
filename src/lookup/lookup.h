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
    Bitboard_t magicHashTable[NUM_HASH_ENTRIES];

    Bitboard_t slidingCheckmasks[NUM_SQUARES][NUM_SQUARES];
    Bitboard_t pawnCheckmasks[2][NUM_SQUARES]; // different for each color

    Bitboard_t directionalRays[NUM_SQUARES][NUM_DIRECTIONS];

    Square_t ksCastleSquares[2];
    Square_t qsCastleSquares[2];

    // EVAL
    Bitboard_t flatPawnShield[2][NUM_SQUARES];
    Bitboard_t pointedPawnShield[2][NUM_SQUARES];

    Bitboard_t outerKingZones[2][NUM_SQUARES];
} Lookup_t;

void InitLookupTables();

Bitboard_t GetSingleBitset(Square_t square);

Bitboard_t GetKnightAttackSet(Square_t square);

Bitboard_t GetKingAttackSet(Square_t square);

Bitboard_t GetRookAttackSet(Square_t square, Bitboard_t empty);

Bitboard_t GetBishopAttackSet(Square_t square, Bitboard_t empty);

Bitboard_t GetSlidingCheckmask(Square_t kingSquare, Square_t slidingPieceSquare);

Bitboard_t GetPawnCheckmask(Square_t kingSquare, Color_t color);

Bitboard_t GetDirectionalRay(Square_t square, Direction_t direction);

Square_t GetKingsideCastleSquare(Color_t color);

Square_t GetQueensideCastleSquare(Color_t color);

Bitboard_t GetFlatPawnShield(Square_t square, Color_t color);

Square_t GetPointedPawnShield(Square_t square, Color_t color);

Bitboard_t GetOuterKingZone(Square_t square, Color_t color);

#endif