#include <stdio.h>

#include "pieces.h"
#include "lookup.h"
#include "board_constants.h"

#define WHITE_PROMOTION_RANK RANK_8
#define BLACK_PROMOTION_RANK RANK_1
#define NOT_WHITE_PROMOTION_RANK C64(~0xff00000000000000)
#define NOT_BLACK_PROMOTION_RANK C64(~0x00000000000000ff)

// PAWNS
Bitboard_t WhiteSinglePushTargets(Bitboard_t wPawns, Bitboard_t empty) {
   return NortOne(wPawns) & empty;
}

Bitboard_t BlackSinglePushTargets(Bitboard_t bPawns, Bitboard_t empty) {
   return SoutOne(bPawns) & empty;
}

Bitboard_t WhiteDoublePushTargets(Bitboard_t wPawns, Bitboard_t empty) {
   Bitboard_t singlePushs = WhiteSinglePushTargets(wPawns, empty);
   return NortOne(singlePushs) & empty & RANK_4;
}

Bitboard_t BlackDoublePushTargets(Bitboard_t bPawns, Bitboard_t empty) {
   Bitboard_t singlePushs = BlackSinglePushTargets(bPawns, empty);
   return SoutOne(singlePushs) & empty & RANK_5;
}

Bitboard_t WhiteEastCaptureTargets(Bitboard_t wPawns, Bitboard_t enemyPieces) {
   return NoEaOne(wPawns) & enemyPieces;
}

Bitboard_t BlackEastCaptureTargets(Bitboard_t bPawns, Bitboard_t enemyPieces) {
   return SoEaOne(bPawns) & enemyPieces;
}

Bitboard_t WhiteWestCaptureTargets(Bitboard_t wPawns, Bitboard_t enemyPieces) {
   return NoWeOne(wPawns) & enemyPieces;
}

Bitboard_t BlackWestCaptureTargets(Bitboard_t bPawns, Bitboard_t enemyPieces) {
   return SoWeOne(bPawns) & enemyPieces;
}

Bitboard_t FilterWhitePromotions(Bitboard_t* whiteMoveset) {
   Bitboard_t promotions = (*whiteMoveset) & WHITE_PROMOTION_RANK;
   (*whiteMoveset) &= NOT_WHITE_PROMOTION_RANK;
   return promotions;
}

Bitboard_t FilterBlackPromotions(Bitboard_t* blackMoveset) {
   Bitboard_t promotions = (*blackMoveset) & BLACK_PROMOTION_RANK;
   (*blackMoveset) &= NOT_BLACK_PROMOTION_RANK;
   return promotions;
}

// KNIGHTS
Bitboard_t KnightMoveTargets(Square_t square, Bitboard_t filter) {
   return GetKnightAttackSet(square) & filter;
}

// KINGS
Bitboard_t KingMoveTargets(Square_t square, Bitboard_t filter) {
   return GetKingAttackSet(square) & filter;
}

// ROOKS
Bitboard_t RookMoveTargets(Square_t square, Bitboard_t empty, Bitboard_t filter) {
   return GetRookAttackSet(square, empty) & filter;
}

// BISHOPS
Bitboard_t BishopMoveTargets(Square_t square, Bitboard_t empty, Bitboard_t filter) {
   return GetBishopAttackSet(square, empty) & filter;
}