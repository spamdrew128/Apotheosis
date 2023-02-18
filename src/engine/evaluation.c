#include "evaluation.h"

typedef uint8_t PieceCount_t;
typedef int32_t Centipawns_t;

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