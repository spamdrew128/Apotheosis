#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"
#include "eval_helpers.h"
#include "util_macros.h"

static Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = { { PASSED_PAWN_MG_PST }, { PASSED_PAWN_EG_PST } }; 

void PassedPawns(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    const Bitboard_t wFrontSpan = WhiteForwardFill(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackForwardFill(boardInfo->pawns[black]);

    const Bitboard_t wPawnBlocks = wFrontSpan | EastOne(wFrontSpan) | WestOne(wFrontSpan);
    const Bitboard_t bPawnBlocks = bFrontSpan | EastOne(bFrontSpan) | WestOne(bFrontSpan);

    Bitboard_t wPassers = boardInfo->pawns[white] & ~bPawnBlocks;
    Bitboard_t bPassers = boardInfo->pawns[black] & ~wPawnBlocks;

    while(wPassers) {
        Square_t sq = MIRROR(LSB(wPassers));
        *mgScore += passerBonus[mg_phase][sq];
        *egScore += passerBonus[eg_phase][sq];
        ResetLSB(&wPassers);
    }
    while(bPassers) {
        Square_t sq = LSB(bPassers);
        *mgScore -= passerBonus[mg_phase][sq];
        *egScore -= passerBonus[eg_phase][sq];
        ResetLSB(&bPassers);
    }
}