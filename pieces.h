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

#define WhiteEastEnPassantTargets(wPawns, wEnPassantSquares) \
    WhiteEastCaptureTargets(wPawns, wEnPassantSquares)

#define BlackEastEnPassantTargets(bPawns, bEnPassantSquares) \
    BlackEastCaptureTargets(bPawns, bEnPassantSquares)

#define WhiteWestEnPassantTargets(wPawns, wEnPassantSquares) \
    WhiteWestCaptureTargets(wPawns, wEnPassantSquares)

#define BlackWestEnPassantTargets(bPawns, bEnPassantSquares) \
    BlackWestCaptureTargets(bPawns, bEnPassantSquares)

// KNIGHTS
Bitboard_t KnightMoveTargets(Square_t square, Bitboard_t empty);

Bitboard_t KnightCaptureTargets(Square_t square, Bitboard_t enemyPieces);

#endif