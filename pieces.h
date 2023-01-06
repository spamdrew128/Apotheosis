#ifndef __PIECES_H__
#define __PIECES_H__

#include "bitboards.h"

// PAWNS
Bitboard_t WhiteSinglePushTargets(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t WhiteDoublePushTargets(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t BlackSinglePushTargets(Bitboard_t bPawns, Bitboard_t empty);

Bitboard_t BlackDoublePushTargets(Bitboard_t bPawns, Bitboard_t empty);

Bitboard_t WhitePushablePawns(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t BlackPushablePawns(Bitboard_t bPawns, Bitboard_t empty);

Bitboard_t WhiteDblPushablePawns(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t BlackDblPushablePawns(Bitboard_t bPawns, Bitboard_t empty);

#endif