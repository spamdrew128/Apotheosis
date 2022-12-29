#ifndef __BITBOARDS_H__
#define __BITBOARDS_H__

#include "board_constants.h"

enum side {
    white,
    black
};

typedef struct {
    Bitboard_t allPieces[2];

    Bitboard_t pawns[2];
    Bitboard_t knights[2];
    Bitboard_t bishops[2];
    Bitboard_t rooks[2];
    Bitboard_t queens[2];
    Bitboard_t kings[2];
} BoardInfo_t;

Bitboard_t NortOne (Bitboard_t b);
Bitboard_t NoEaOne (Bitboard_t b);
Bitboard_t EastOne (Bitboard_t b);
Bitboard_t SoEaOne (Bitboard_t b);
Bitboard_t SoutOne (Bitboard_t b);
Bitboard_t SoWeOne (Bitboard_t b);
Bitboard_t WestOne (Bitboard_t b);
Bitboard_t NoWeOne (Bitboard_t b);

typedef int Population_t;
Population_t PopulationCount(Bitboard_t b);

Square_t LSB(Bitboard_t b);

#define SetBits(bitboard, mask) bitboard |= mask
#define ResetBits(bitboard, mask) bitboard &= ~mask
#define ToggleBits(bitboard, mask) bitboard ^= mask

#endif
