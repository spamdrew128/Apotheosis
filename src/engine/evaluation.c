#include "evaluation.h"
#include "PST.h"

typedef uint8_t PieceCount_t;
typedef int32_t Centipawns_t;

static Centipawns_t pawnPST[2][NUM_SQUARES] = { PAWN_MG_PST, PAWN_EG_PST };
static Centipawns_t knightPST[2][NUM_SQUARES] = { KNIGHT_MG_PST, KNIGHT_EG_PST };
static Centipawns_t bishopPST[2][NUM_SQUARES] = { BISHOP_MG_PST, BISHOP_EG_PST };
static Centipawns_t rookPST[2][NUM_SQUARES] = { ROOK_MG_PST, ROOK_EG_PST };
static Centipawns_t queenPST[2][NUM_SQUARES] = { QUEEN_MG_PST, QUEEN_EG_PST };
static Centipawns_t kingPST[2][NUM_SQUARES] = { KING_MG_PST, KING_EG_PST };

static Centipawns_t SingleTypeMaterialBalanceScore(Bitboard_t infoField[2], Centipawns_t value) {
    return value * (PopCount(infoField[white]) - PopCount(infoField[black]));
}

static Centipawns_t MaterialBalance(BoardInfo_t* boardInfo) {
    return 
        SingleTypeMaterialBalanceScore(boardInfo->knights, knight_value) +
        SingleTypeMaterialBalanceScore(boardInfo->bishops, bishop_value) +
        SingleTypeMaterialBalanceScore(boardInfo->rooks, rook_value) +
        SingleTypeMaterialBalanceScore(boardInfo->queens, queen_value) +
        SingleTypeMaterialBalanceScore(boardInfo->pawns, pawn_value);
}

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo) {
    EvalScore_t eval = MaterialBalance(boardInfo);
    return boardInfo->colorToMove == white ? eval : -eval;
}