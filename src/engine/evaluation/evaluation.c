#include <assert.h>

#include "evaluation.h"
#include "eval_constants.h"
#include "util_macros.h"
#include "legals.h"
#include "pawn_structure.h"

static Centipawns_t piecePSTs[NUM_PIECES][NUM_PHASES][NUM_SQUARES] = { KNIGHT_PST, BISHOP_PST, ROOK_PST, QUEEN_PST, PAWN_PST, KING_PST };
static Phase_t gamePhaseLookup[6] = { KNIGHT_PHASE_VALUE, BISHOP_PHASE_VALUE, ROOK_PHASE_VALUE, QUEEN_PHASE_VALUE, PAWN_PHASE_VALUE, KING_PHASE_VALUE };

static Centipawns_t bishopPairBonus[NUM_PHASES] = { BISHOP_PAIR_BONUS };

static void PSTEval(
    Bitboard_t infoField[2],
    Piece_t piece,
    Phase_t* gamePhase,
    Centipawns_t* mgScore,
    Centipawns_t* egScore
)
{
    Bitboard_t whitePieces = infoField[white];
    Bitboard_t blackPieces = infoField[black];

    while(whitePieces) {
        Square_t sq = MIRROR(LSB(whitePieces));
        *mgScore += piecePSTs[piece][mg_phase][sq];
        *egScore += piecePSTs[piece][eg_phase][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        *mgScore -= piecePSTs[piece][mg_phase][sq];
        *egScore -= piecePSTs[piece][eg_phase][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&blackPieces);
    }
}

static void MaterialBalanceAndPSTBonus(BoardInfo_t* boardInfo, Phase_t* phase, Centipawns_t* mgScore, Centipawns_t* egScore) {
    PSTEval(boardInfo->knights, knight, phase, mgScore, egScore);
    PSTEval(boardInfo->bishops, bishop, phase, mgScore, egScore);
    PSTEval(boardInfo->rooks, rook, phase, mgScore, egScore);
    PSTEval(boardInfo->queens, queen, phase, mgScore, egScore);
    PSTEval(boardInfo->pawns, pawn, phase, mgScore, egScore);
    PSTEval(boardInfo->kings, king, phase, mgScore, egScore);
}

static void BishopPairBonus(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    if(PopCount(boardInfo->bishops[white]) >= 2) {
        *mgScore += bishopPairBonus[mg_phase];
        *egScore += bishopPairBonus[eg_phase];
    }
    if(PopCount(boardInfo->bishops[black]) >= 2) {
        *mgScore -= bishopPairBonus[mg_phase];
        *egScore -= bishopPairBonus[eg_phase];
    }
}

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo) {
    Phase_t phase = 0;
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;

    MaterialBalanceAndPSTBonus(boardInfo, &phase, &mgScore, &egScore);
    BishopPairBonus(boardInfo, &mgScore, &egScore);
    PassedPawnBonus(boardInfo, &mgScore, &egScore);
    OpenFileBonus(boardInfo, &mgScore, &egScore);

    EvalScore_t eval = (mgScore*phase + egScore*(PHASE_MAX - phase)) / PHASE_MAX;

    return boardInfo->colorToMove == white ? eval : -eval;
}