#ifndef __LEGALS_H__
#define __LEGALS_H__

#include <stdbool.h>

#include "board_constants.h"
#include "bitboards.h"

Bitboard_t WhiteUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t BlackUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t KingLegalMoves(Bitboard_t kingMoves, Bitboard_t unsafeSquares);

Bitboard_t CastlingMoves(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color);

Bitboard_t DefineCheckmask(Bitboard_t enemySliders, bool inCheck);

#endif