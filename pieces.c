#include <stdio.h>

#include "pieces.h"
#include "lookup.h"
#include "board_constants.h"

// PAWNS
Bitboard_t WhiteSinglePushTargets(Bitboard_t wPawns, Bitboard_t empty) {
   return NortOne(wPawns) & empty;
}

Bitboard_t BlackSinglePushTargets(Bitboard_t bPawns, Bitboard_t empty) {
   return SoutOne(bPawns) & empty;
}

Bitboard_t WhiteDoublePushTargets(Bitboard_t wPawns, Bitboard_t empty) {
   Bitboard_t singlePushs = WhiteSinglePushTargets(wPawns, empty);
   return NortOne(singlePushs) & empty & rank_4;
}

Bitboard_t BlackDoublePushTargets(Bitboard_t bPawns, Bitboard_t empty) {
   Bitboard_t singlePushs = BlackSinglePushTargets(bPawns, empty);
   return SoutOne(singlePushs) & empty & rank_5;
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

// KNIGHTS
Bitboard_t KnightMoveTargets(Square_t square, Bitboard_t empty) {
   return GetKnightAttacks(square) & empty;
}

Bitboard_t KnightCaptureTargets(Square_t square, Bitboard_t enemyPieces) {
   return GetKnightAttacks(square) & enemyPieces;
}

// KINGS
Bitboard_t KingMoveTargets(Square_t square, Bitboard_t empty) {
   return GetKingAttacks(square) & empty;
}

Bitboard_t KingCaptureTargets(Square_t square, Bitboard_t enemyPieces) {
   return GetKingAttacks(square) & enemyPieces;
}

// ROOKS
Bitboard_t RookMoveTargets(Square_t square, Bitboard_t empty) {
   MagicEntry_t magicEntry = GetRookMagicEntry(square);
   Bitboard_t blockers = magicEntry.mask & ~empty;
   return GetSlidingAttackSet(magicEntry, blockers) & empty;
}

Bitboard_t RookCaptureTargets(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces) {
   MagicEntry_t magicEntry = GetRookMagicEntry(square);
   Bitboard_t blockers = magicEntry.mask & ~empty;
   return GetSlidingAttackSet(magicEntry, blockers) & enemyPieces;
}

// BISHOPS
Bitboard_t BishopMoveTargets(Square_t square, Bitboard_t empty) {
   MagicEntry_t magicEntry = GetBishopMagicEntry(square);
   Bitboard_t blockers = magicEntry.mask & ~empty;
   return GetSlidingAttackSet(magicEntry, blockers) & empty;
}

Bitboard_t BishopCaptureTargets(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces) {
   MagicEntry_t magicEntry = GetBishopMagicEntry(square);
   Bitboard_t blockers = magicEntry.mask & ~empty;
   return GetSlidingAttackSet(magicEntry, blockers) & enemyPieces;
}

// QUEENS
Bitboard_t QueenMoveTargets(Square_t square, Bitboard_t empty) {
   return RookMoveTargets(square, empty) | BishopMoveTargets(square, empty);
}

Bitboard_t QueenCaptureTargets(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces) {
   return RookCaptureTargets(square, empty, enemyPieces) | BishopCaptureTargets(square, empty, enemyPieces);
}