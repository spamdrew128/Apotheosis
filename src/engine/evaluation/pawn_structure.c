#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"
#include "eval_helpers.h"
#include "util_macros.h"

static Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = { { PASSED_PAWN_MG_PST }, { PASSED_PAWN_EG_PST } };
static Centipawns_t blockedPasserPenalty[NUM_PHASES][NUM_RANKS] = { { BLOCKED_PASSERS_MG }, { BLOCKED_PASSERS_EG } };

static Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = { { PASSED_PAWN_MG_PST }, { PASSED_PAWN_EG_PST } };
static Centipawns_t blockedPasserPenalty[NUM_PHASES][NUM_RANKS] = { { BLOCKED_PASSERS_MG }, { BLOCKED_PASSERS_EG } };

static void PassedPawns(
    BoardInfo_t* boardInfo,
    Centipawns_t* mgScore,
    Centipawns_t* egScore,
    const Bitboard_t wFrontSpan,
    const Bitboard_t bFrontSpan
)
{
    const Bitboard_t wPawnBlocks = wFrontSpan | EastOne(wFrontSpan) | WestOne(wFrontSpan);
    const Bitboard_t bPawnBlocks = bFrontSpan | EastOne(bFrontSpan) | WestOne(bFrontSpan);

    Bitboard_t wPassers = boardInfo->pawns[white] & ~bPawnBlocks;
    Bitboard_t bPassers = boardInfo->pawns[black] & ~wPawnBlocks;

    Bitboard_t piecesBlockingWhite = NortOne(wPassers) & boardInfo->allPieces[black];
    Bitboard_t piecesBlockingBlack = SoutOne(bPassers) & boardInfo->allPieces[white];

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

    while(piecesBlockingWhite) {
        Rank_t rank = (MIRROR(LSB(piecesBlockingWhite))) / 8;
        *mgScore += blockedPasserPenalty[mg_phase][rank];
        *egScore += blockedPasserPenalty[eg_phase][rank];
        ResetLSB(&piecesBlockingWhite);
    }
    while(piecesBlockingBlack) {
        Rank_t rank = LSB(piecesBlockingBlack) / 8;
        *mgScore -= blockedPasserPenalty[mg_phase][rank];
        *egScore -= blockedPasserPenalty[eg_phase][rank];
        ResetLSB(&piecesBlockingBlack);
    }
}

static void RookOpenFile(
    BoardInfo_t* boardInfo,
    Centipawns_t* mgScore,
    Centipawns_t* egScore,
    const Bitboard_t wFrontSpan,
    const Bitboard_t bFrontSpan
)
{

}

void PawnStuff(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    const Bitboard_t wFrontSpan = WhiteForwardFill(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackForwardFill(boardInfo->pawns[black]);

    PassedPawns(boardInfo, mgScore, egScore, wFrontSpan, bFrontSpan);
}