#include <assert.h>

#include "evaluation.h"
#include "eval_constants.h"
#include "util_macros.h"
#include "legals.h"
#include "pawn_structure.h"

enum {
    QS_PST_MASK = C64(0x0F0F0F0F0F0F0F0F),
    KS_PST_MASK = C64(0xF0F0F0F0F0F0F0F0),
};

static Phase_t gamePhaseLookup[6] = { KNIGHT_PHASE_VALUE, BISHOP_PHASE_VALUE, ROOK_PHASE_VALUE, QUEEN_PHASE_VALUE, PAWN_PHASE_VALUE, KING_PHASE_VALUE };

static Score_t piecePSTs[NUM_PST_BUCKETS][NUM_PIECES][NUM_SQUARES] = {
    { KNIGHT_PST, BISHOP_PST, ROOK_PST, QUEEN_PST, PAWN_PST, KING_PST }, 
    { KNIGHT_PST, BISHOP_PST, ROOK_PST, QUEEN_PST, PAWN_PST, KING_PST }
};
static Score_t bishopPairBonus = BISHOP_PAIR_BONUS;

static int PSTBucketIndex(BoardInfo_t* boardInfo, Color_t color) {
    // indexed by opponent king position
    return (bool)(boardInfo->kings[!color] & QS_PST_MASK);
}

static void PSTEval(
    Bitboard_t infoField[2],
    const int wBucket,
    const int bBucket,
    Piece_t piece,
    Phase_t* gamePhase,
    Score_t* score
)
{
    Bitboard_t whitePieces = infoField[white];
    Bitboard_t blackPieces = infoField[black];

    while(whitePieces) {
        Square_t sq = MIRROR(LSB(whitePieces));
        *score += piecePSTs[wBucket][piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        *score -= piecePSTs[bBucket][piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&blackPieces);
    }
}

static void MaterialBalanceAndPSTBonus(BoardInfo_t* boardInfo, Phase_t* phase, Score_t* score) {
    const int whiteBucket = PSTBucketIndex(boardInfo, white);
    const int blackBucket = PSTBucketIndex(boardInfo, black);

    PSTEval(boardInfo->knights, whiteBucket, blackBucket, knight, phase, score);
    PSTEval(boardInfo->bishops, whiteBucket, blackBucket,  bishop, phase, score);
    PSTEval(boardInfo->rooks, whiteBucket, blackBucket,  rook, phase, score);
    PSTEval(boardInfo->queens, whiteBucket, blackBucket,  queen, phase, score);
    PSTEval(boardInfo->pawns, whiteBucket, blackBucket,  pawn, phase, score);
    PSTEval(boardInfo->kings, whiteBucket, blackBucket,  king, phase, score);
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