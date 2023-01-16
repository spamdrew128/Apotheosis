#ifndef __BITBOARDS_H__
#define __BITBOARDS_H__

#include "board_constants.h"

typedef struct {
    Bitboard_t allPieces[2];
    Bitboard_t empty;

    Bitboard_t pawns[2];
    Bitboard_t knights[2];
    Bitboard_t bishops[2];
    Bitboard_t rooks[2];
    Bitboard_t queens[2];
    Bitboard_t kings[2];

    Bitboard_t castleSquares[2];
} BoardInfo_t;

#define UpdateAllPieces(info) \
    for(int i = 0; i < 2; i++) { \
        info->allPieces[i] = \
            info->pawns[i] | \
            info->knights[i] | \
            info->bishops[i] | \
            info->rooks[i] | \
            info->queens[i] | \
            info->kings[i]; \
    }

#define UpdateEmpty(info) info->empty = ~(info->allPieces[white] | info->allPieces[black])

void InitBoardInfo(BoardInfo_t* info);

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
