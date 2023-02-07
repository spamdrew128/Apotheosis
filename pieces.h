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

Bitboard_t FilterWhitePromotions(Bitboard_t* whiteMoveset);

Bitboard_t FilterBlackPromotions(Bitboard_t* blackMoveset);

#define WhiteEastEnPassantTargets(wPawns, wEnPassantSquares) \
    WhiteEastCaptureTargets(wPawns, wEnPassantSquares)

#define BlackEastEnPassantTargets(bPawns, bEnPassantSquares) \
    BlackEastCaptureTargets(bPawns, bEnPassantSquares)

#define WhiteWestEnPassantTargets(wPawns, wEnPassantSquares) \
    WhiteWestCaptureTargets(wPawns, wEnPassantSquares)

#define BlackWestEnPassantTargets(bPawns, bEnPassantSquares) \
    BlackWestCaptureTargets(bPawns, bEnPassantSquares)

// KNIGHTS
Bitboard_t KnightMoveTargetsNew(Square_t square, Bitboard_t filter);

// KINGS
Bitboard_t KingMoveTargets(Square_t square, Bitboard_t empty);

Bitboard_t KingCaptureTargets(Square_t square, Bitboard_t enemyPieces);

Bitboard_t KingMoveTargetsNew(Square_t square, Bitboard_t filter);

// ROOKS
Bitboard_t RookMoveTargets(Square_t square, Bitboard_t empty);

Bitboard_t RookCaptureTargets(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces);

Bitboard_t RookMoveTargetsNew(Square_t square, Bitboard_t empty, Bitboard_t filter);

// BISHOPS
Bitboard_t BishopMoveTargets(Square_t square, Bitboard_t empty);

Bitboard_t BishopCaptureTargets(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces);

Bitboard_t BishopMoveTargetsNew(Square_t square, Bitboard_t empty, Bitboard_t filter);

// QUEENS
Bitboard_t QueenMoveTargets(Square_t square, Bitboard_t empty);

Bitboard_t QueenCaptureTargets(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces);

#endif