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
