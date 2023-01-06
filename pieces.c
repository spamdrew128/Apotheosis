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

Bitboard_t WhitePushablePawns(Bitboard_t wPawns, Bitboard_t empty) {
   return SoutOne(empty) & wPawns;
}

Bitboard_t BlackPushablePawns(Bitboard_t bPawns, Bitboard_t empty) {
   return NortOne(empty) & bPawns;
}

Bitboard_t WhiteDblPushablePawns(Bitboard_t wPawns, Bitboard_t empty) {
   Bitboard_t emptyRank3 = SoutOne(empty & rank_4) & empty;
   return WhitePushablePawns(wPawns, emptyRank3);
}

Bitboard_t BlackDblPushablePawns(Bitboard_t bPawns, Bitboard_t empty) {
   Bitboard_t emptyRank6 = NortOne(empty & rank_5) & empty;
   return BlackPushablePawns(bPawns, emptyRank6);
}