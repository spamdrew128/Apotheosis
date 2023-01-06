#ifndef __PIECES_H__
#define __PIECES_H__

#include "bitboards.h"

// PAWNS
Bitboard_t WhiteSinglePushTargets(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t WhiteDoublePushTargets(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t BlackSinglePushTargets(Bitboard_t bPawns, Bitboard_t empty);

Bitboard_t BlackDoublePushTargets(Bitboard_t bPawns, Bitboard_t empty);

#endif