#ifndef __PIECES_H__
#define __PIECES_H__

#include "bitboards.h"

// PAWNS
Bitboard_t WhiteSinglePushTargets(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t WhiteDoublePushTargets(Bitboard_t wPawns, Bitboard_t empty);

Bitboard_t BlackSinglePushTargets(Bitboard_t bPawns, Bitboard_t empty);

Bitboard_t BlackDoublePushTargets(Bitboard_t bPawns, Bitboard_t empty);

Bitboard_t WhiteEastCaptureTargets(Bitboard_t wPawns, Bitboard_t enemyPieces);

Bitboard_t BlackEastCaptureTargets(Bitboard_t bPawns, Bitboard_t enemyPieces);

Bitboard_t WhiteWestCaptureTargets(Bitboard_t wPawns, Bitboard_t enemyPieces);

Bitboard_t BlackWestCaptureTargets(Bitboard_t bPawns, Bitboard_t enemyPieces);

#define WhiteEastEnPassantTargets(wPawns, wPotentialTargets) \
    WhiteEastCaptureTargets(wPawns, wPotentialTargets)

#define BlackEastEnPassantTargets(bPawns, bPotentialTargets) \
    BlackEastCaptureTargets(bPawns, bPotentialTargets)

#define WhiteWestEnPassantTargets(wPawns, wPotentialTargets) \
    WhiteWestCaptureTargets(wPawns, wPotentialTargets)

#define BlackWestEnPassantTargets(bPawns, bPotentialTargets) \
    BlackWestCaptureTargets(bPawns, bPotentialTargets)

#endif