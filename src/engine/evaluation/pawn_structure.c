#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"
#include "eval_helpers.h"

static Centipawns_t passerBonus[2] = { PASSED_PAWN_BONUS }; 

void PassedPawns(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    const Bitboard_t wFrontSpan = WhiteForwardFill(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackForwardFill(boardInfo->pawns[black]);

    const Bitboard_t wPawnBlocks = wFrontSpan | EastOne(wFrontSpan) | WestOne(wFrontSpan);
    const Bitboard_t bPawnBlocks = bFrontSpan | EastOne(bFrontSpan) | WestOne(bFrontSpan);

    const Bitboard_t wPassers = boardInfo->pawns[white] & ~bPawnBlocks;
    const Bitboard_t bPassers = boardInfo->pawns[black] & ~wPawnBlocks;

    const int8_t passerCount = PopCount(wPassers) - PopCount(bPassers);

    *mgScore += passerCount * passerBonus[mg_phase];
    *egScore += passerCount * passerBonus[eg_phase];
}