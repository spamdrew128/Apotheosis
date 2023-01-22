#ifndef __LEGALS_H__
#define __LEGALS_H__

#include <stdbool.h>

#include "board_constants.h"
#include "bitboards.h"

Bitboard_t WhiteUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t BlackUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t KingLegalMoves(Bitboard_t kingMoves, Bitboard_t unsafeSquares);

Bitboard_t CastlingMoves(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color);

#define InCheck(boardInfoAddress, unsafeSquares, color) unsafeSquares & (boardInfo)->kings[color]

Bitboard_t DefineCheckmask(BoardInfo_t* boardInfo, Color_t color);

bool IsDoubleCheck(BoardInfo_t* boardInfo, Bitboard_t checkmask, Color_t color);

#endif