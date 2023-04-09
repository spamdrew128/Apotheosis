#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = {0};
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = {0};
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = {0};
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = {0};

static Score_t ComputeKnights(Bitboard_t knights, Bitboard_t safe) {
    Score_t score = 0;
    while(knights) {
        Square_t sq = LSB(knights);
        Bitboard_t safeMoves = GetKnightAttackSet(sq) & safe;
        score += knightMobility[PopCount(safeMoves)];
        ResetLSB(&knights);
    }
    return score;
}

static Score_t ComputeBishops(Bitboard_t bishops, Bitboard_t safe, Bitboard_t d12Empty) {
    Score_t score = 0;
    while(bishops) {
        Square_t sq = LSB(bishops);
        Bitboard_t safeMoves = GetBishopAttackSet(sq, d12Empty) & safe;
        score += bishopMobility[PopCount(safeMoves)];
        ResetLSB(&bishops);
    }
    return score;
}

static Score_t ComputeRooks(Bitboard_t rooks, Bitboard_t safe, Bitboard_t hvEmpty) {
    Score_t score = 0;
    while(rooks) {
        Square_t sq = LSB(rooks);
        Bitboard_t safeMoves = GetRookAttackSet(sq, hvEmpty) & safe;
        score += rookMobility[PopCount(safeMoves)];
        ResetLSB(&rooks);
    }
    return score;    
}

static Score_t ComputeQueens(Bitboard_t queens, Bitboard_t safe, Bitboard_t hvEmpty, Bitboard_t d12Empty) {
    Score_t score = 0;
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t safeD12Moves = GetBishopAttackSet(sq, d12Empty) & safe;
        Bitboard_t safeHvMoves = GetRookAttackSet(sq, hvEmpty) & safe;
        score += queenMobility[PopCount(safeD12Moves) + PopCount(safeHvMoves)];
        ResetLSB(&queens);
    }
    return score;    
}

void MobilityEval(BoardInfo_t* boardInfo, Score_t* score) {
    const Bitboard_t wPawnAttacks = 
        NoEaOne(boardInfo->pawns[white]) | 
        NoWeOne(boardInfo->pawns[white]);
    const Bitboard_t bPawnAttacks = 
        SoEaOne(boardInfo->pawns[black]) |
        SoWeOne(boardInfo->pawns[black]);

    const Bitboard_t wSafe = ~bPawnAttacks;
    const Bitboard_t bSafe = ~wPawnAttacks;


}