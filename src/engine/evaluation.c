#include <assert.h>
#include <stdint.h>

#include "evaluation.h"
#include "eval_constants.h"
#include "util_macros.h"
#include "legals.h"

static Centipawns_t midgamePST[] = { SUPER_PST_MG };
static Centipawns_t endgamePST[] = { SUPER_PST_EG };
static Phase_t gamePhaseLookup[6] = { KNIGHT_PHASE_VALUE, BISHOP_PHASE_VALUE, ROOK_PHASE_VALUE, QUEEN_PHASE_VALUE, PAWN_PHASE_VALUE, KING_PHASE_VALUE };

static Centipawns_t bishopPairBonus[NUM_PHASES] = { BISHOP_PAIR_BONUS };

typedef uint8_t Bucket_t;
static int FlatPSTIndex(Bucket_t w, Bucket_t b, Color_t c, Piece_t p, Square_t sq) {
    return (8*2*6*64)*w + (2*6*64)*b + (6*64)*c + (64)*p + sq;
}

static void PSTEval(
    Bitboard_t infoField[2],
    const Bucket_t wKing,
    const Bucket_t bKing,
    Piece_t piece,
    Phase_t* gamePhase,
    Centipawns_t* mgScore,
    Centipawns_t* egScore
)
{
    Bitboard_t whitePieces = infoField[white];
    Bitboard_t blackPieces = infoField[black];

    while(whitePieces) {
        int index = FlatPSTIndex(wKing, bKing, white, piece, LSB(whitePieces));

        *mgScore += midgamePST[index];
        *egScore += endgamePST[index];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        int index = FlatPSTIndex(wKing, bKing, black, piece, LSB(blackPieces));

        *mgScore -= midgamePST[index];
        *egScore -= endgamePST[index];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&blackPieces);
    }
}

static void MaterialBalanceAndPSTBonus(BoardInfo_t* boardInfo, Phase_t* phase, Centipawns_t* mgScore, Centipawns_t* egScore) {
    Bucket_t wKing = KingSquare(boardInfo, white) / 8;
    Bucket_t bKing = KingSquare(boardInfo, black) / 8;

    PSTEval(boardInfo->knights, wKing, bKing, knight, phase, mgScore, egScore);
    PSTEval(boardInfo->bishops, wKing, bKing, bishop, phase, mgScore, egScore);
    PSTEval(boardInfo->rooks, wKing, bKing, rook, phase, mgScore, egScore);
    PSTEval(boardInfo->queens, wKing, bKing, queen, phase, mgScore, egScore);
    PSTEval(boardInfo->pawns, wKing, bKing, pawn, phase, mgScore, egScore);
    PSTEval(boardInfo->kings, wKing, bKing, king, phase, mgScore, egScore);
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

    EvalScore_t eval = (mgScore*phase + egScore*(PHASE_MAX - phase)) / PHASE_MAX;

    return boardInfo->colorToMove == white ? eval : -eval;
}