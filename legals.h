#ifndef __LEGALS_H__
#define __LEGALS_H__

#include <stdbool.h>

#include "board_info.h"
#include "board_constants.h"
#include "bitboards.h"
#include "castling.h"

// mostly everything I need for movegen.c

typedef struct
{
    Bitboard_t hv;
    Bitboard_t d12;
    Bitboard_t all;
} PinmaskContainer_t;

Bitboard_t WhiteUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t BlackUnsafeSquares(BoardInfo_t* boardInfo);

Bitboard_t KingLegalMoves(Bitboard_t kingMoves, Bitboard_t unsafeSquares);

bool CanCastleQueenside(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, CastleRights_t castleRights, Color_t color);

bool CanCastleKingside(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, CastleRights_t castleRights, Color_t color);

Bitboard_t DefineCheckmask(BoardInfo_t* boardInfo, Color_t color);

bool InCheck(Bitboard_t kingBitboard, Bitboard_t unsafeSquares);

bool IsDoubleCheck(BoardInfo_t* boardInfo, Bitboard_t checkmask, Color_t color);

PinmaskContainer_t DefinePinmasks(BoardInfo_t* boardInfo, Color_t color);

bool EastEnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t friendlyPawnLocation, Color_t color);

bool WestEnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t friendlyPawnLocation, Color_t color);

#endif