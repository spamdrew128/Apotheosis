#ifndef __LEGALS_H__
#define __LEGALS_H__

#include <stdbool.h>

#include "board_info.h"
#include "board_constants.h"
#include "bitboards.h"

// mostly everything I need for movegen.c

typedef struct
{
    Bitboard_t hvPinmask;
    Bitboard_t d12Pinmask;
    Bitboard_t allPinmask;
} PinmaskContainer_t;

Bitboard_t WhiteUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t BlackUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t KingLegalMoves(Bitboard_t kingMoves, Bitboard_t unsafeSquares);

Bitboard_t CastlingMoves(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color);

#define InCheck(boardInfoPtr, unsafeSquares, color) unsafeSquares & (boardInfo)->kings[color]

Bitboard_t DefineCheckmask(BoardInfo_t* boardInfo, Color_t color);

bool IsDoubleCheck(BoardInfo_t* boardInfo, Bitboard_t checkmask, Color_t color);

PinmaskContainer_t DefinePinmasks(BoardInfo_t* boardInfo, Color_t color);

#endif