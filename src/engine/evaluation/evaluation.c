#include <assert.h>

#include "evaluation.h"
#include "eval_constants.h"
#include "util_macros.h"
#include "legals.h"
#include "pawn_structure.h"

static Phase_t gamePhaseLookup[6] = { KNIGHT_PHASE_VALUE, BISHOP_PHASE_VALUE, ROOK_PHASE_VALUE, QUEEN_PHASE_VALUE, PAWN_PHASE_VALUE, KING_PHASE_VALUE };

static Score_t piecePSTs[NUM_PIECES][NUM_SQUARES] = { KNIGHT_PST, BISHOP_PST, ROOK_PST, QUEEN_PST, PAWN_PST, KING_PST };
static Score_t bishopPairBonus = BISHOP_PAIR_BONUS;

static void PSTEval(
    Bitboard_t infoField[2],
    Piece_t piece,
    Phase_t* gamePhase,
    Score_t* score
)
{
    Bitboard_t whitePieces = infoField[white];
    Bitboard_t blackPieces = infoField[black];

    while(whitePieces) {
        Square_t sq = MIRROR(LSB(whitePieces));
        *score += piecePSTs[piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        *score -= piecePSTs[piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&blackPieces);
    }
}

static void MaterialBalanceAndPSTBonus(BoardInfo_t* boardInfo, Phase_t* phase, Score_t* score) {
    PSTEval(boardInfo->knights, knight, phase, score);
    PSTEval(boardInfo->bishops, bishop, phase, score);
    PSTEval(boardInfo->rooks, rook, phase, score);
    PSTEval(boardInfo->queens, queen, phase, score);
    PSTEval(boardInfo->pawns, pawn, phase, score);
    PSTEval(boardInfo->kings, king, phase, score);
}

static void BishopPairBonus(BoardInfo_t* boardInfo, Score_t* score) {
    if(PopCount(boardInfo->bishops[white]) >= 2) {
        *score += bishopPairBonus;
    }
    if(PopCount(boardInfo->bishops[black]) >= 2) {
        *score -= bishopPairBonus;
    }
}

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo) {
    Phase_t phase = 0;
    Score_t score = 0;

    MaterialBalanceAndPSTBonus(boardInfo, &phase, &score);
    BishopPairBonus(boardInfo, &score);
    PassedPawnBonus(boardInfo, &score);
    OpenFileBonus(boardInfo, &score);

    const EvalScore_t mgScore = MgFromScore(score);
    const EvalScore_t egScore = EgFromScore(score);

    EvalScore_t eval = (mgScore*phase + egScore*(PHASE_MAX - phase)) / PHASE_MAX;

    return boardInfo->colorToMove == white ? eval : -eval;
}