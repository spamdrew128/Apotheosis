#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"

static Centipawns_t passerBonus[2] = { PASSED_PAWN_BONUS }; 

static Bitboard_t WhiteFrontSpan(Bitboard_t b) {
    b |= GenShiftNorth(b, 1);
    b |= GenShiftNorth(b, 2);
    b |= GenShiftNorth(b, 4);
    return b;
}

static Bitboard_t BlackFrontSpan(Bitboard_t b) {
    b |= GenShiftSouth(b, 1);
    b |= GenShiftSouth(b, 2);
    b |= GenShiftSouth(b, 4);
    return b;
}

void PassedPawns(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    const Bitboard_t wFrontSpan = WhiteFrontSpan(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackFrontSpan(boardInfo->pawns[black]);

    const Bitboard_t wPawnSpan = wFrontSpan | EastOne(wFrontSpan) | WestOne(wFrontSpan);
    const Bitboard_t bPawnSpan = bFrontSpan | EastOne(bFrontSpan) | WestOne(bFrontSpan);
}