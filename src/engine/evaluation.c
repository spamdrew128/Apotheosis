#include <assert.h>

#include "evaluation.h"
#include "eval_constants.h"
#include "util_macros.h"
#include "legals.h"

enum {
    mobility_weight = 5
};

static Centipawns_t midgamePST[6][NUM_SQUARES] = { KNIGHT_MG_PST, BISHOP_MG_PST, ROOK_MG_PST, QUEEN_MG_PST, PAWN_MG_PST, KING_MG_PST };
static Centipawns_t endgamePST[6][NUM_SQUARES] = { KNIGHT_EG_PST, BISHOP_EG_PST, ROOK_EG_PST, QUEEN_EG_PST, PAWN_EG_PST, KING_EG_PST };
static Phase_t gamePhaseLookup[6] = { KNIGHT_PHASE_VALUE, BISHOP_PHASE_VALUE, ROOK_PHASE_VALUE, QUEEN_PHASE_VALUE, PAWN_PHASE_VALUE, KING_PHASE_VALUE };

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
        *mgScore += midgamePST[piece][sq];
        *egScore += endgamePST[piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        *mgScore -= midgamePST[piece][sq];
        *egScore -= endgamePST[piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&blackPieces);
    }
}

static Centipawns_t MaterialBalanceAndPSTBonus(BoardInfo_t* boardInfo) {
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;
    Phase_t gamePhase = 0;

    PSTEval(boardInfo->knights, knight, &gamePhase, &mgScore, &egScore);
    PSTEval(boardInfo->bishops, bishop, &gamePhase, &mgScore, &egScore);
    PSTEval(boardInfo->rooks, rook, &gamePhase, &mgScore, &egScore);
    PSTEval(boardInfo->queens, queen, &gamePhase, &mgScore, &egScore);
    PSTEval(boardInfo->pawns, pawn, &gamePhase, &mgScore, &egScore);
    PSTEval(boardInfo->pawns, king, &gamePhase, &mgScore, &egScore);

    Phase_t mgPhase = MIN(gamePhase, PHASE_MAX);
    Phase_t egPhase = PHASE_MAX - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / PHASE_MAX; // weighted average
}

static Centipawns_t MobilityEval(BoardInfo_t* boardInfo) {
    Bitboard_t empty = boardInfo->empty;
    Bitboard_t whitePseudolegals = AttackedSquares(boardInfo, empty, white) & ~boardInfo->allPieces[white];
    Bitboard_t blackPseudolegals = AttackedSquares(boardInfo, empty, black) & ~boardInfo->allPieces[black];

    return (PopCount(whitePseudolegals) - PopCount(blackPseudolegals)) * mobility_weight;
}

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo) {
    EvalScore_t eval = 0;
    eval += MaterialBalanceAndPSTBonus(boardInfo);
    eval += MobilityEval(boardInfo);

    return boardInfo->colorToMove == white ? eval : -eval;
}