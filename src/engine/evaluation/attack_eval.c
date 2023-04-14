#include <stdint.h>

#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"

static const Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static const Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static const Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static const Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

typedef Score_t AttackScore_t;
typedef Score_t DefenseScore_t;
static const AttackScore_t innerAttacks[NUM_PIECES] = INNER_ATTACKS;
static const AttackScore_t outerAttacks[NUM_PIECES] = OUTER_ATTACKS;

static const DefenseScore_t innerDefense[NUM_PIECES] = INNER_DEFENSE;
static const DefenseScore_t outerDefense[NUM_PIECES] = OUTER_DEFENSE; 
static const DefenseScore_t kingAiriness = KING_AIRINESS;
static const DefenseScore_t flatPawnShield = FLAT_PAWN_SHIELD;
static const DefenseScore_t pointedPawnShield = POINTED_PAWN_SHIELD;

static Score_t ComputeKnights(
    Bitboard_t knights,
    Bitboard_t availible
)
{
    Score_t score = 0;
    while(knights) {
        Square_t sq = LSB(knights);
        Bitboard_t moves = GetKnightAttackSet(sq) & availible;
        score += knightMobility[PopCount(moves)];
        ResetLSB(&knights);
    }
    return score;
}

static Score_t ComputeBishops(
    Bitboard_t bishops,
    Bitboard_t availible,
    Bitboard_t d12Empty
)
{
    Score_t score = 0;
    while(bishops) {
        Square_t sq = LSB(bishops);
        Bitboard_t moves = GetBishopAttackSet(sq, d12Empty) & availible;
        score += bishopMobility[PopCount(moves)];
        ResetLSB(&bishops);
    }
    return score;
}

static Score_t ComputeRooks(
    Bitboard_t rooks, 
    Bitboard_t availible,
    Bitboard_t hvEmpty
)
{
    Score_t score = 0;
    while(rooks) {
        Square_t sq = LSB(rooks);
        Bitboard_t moves = GetRookAttackSet(sq, hvEmpty) & availible;
        score += rookMobility[PopCount(moves)];
        ResetLSB(&rooks);
    }
    return score;    
}

static Score_t ComputeQueens(
    Bitboard_t queens,
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    Bitboard_t d12Empty
)
{
    Score_t score = 0;
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t d12Moves = GetBishopAttackSet(sq, d12Empty) & availible;
        Bitboard_t hvMoves = GetRookAttackSet(sq, hvEmpty) & availible;
        score += queenMobility[PopCount(d12Moves) + PopCount(hvMoves)];
        ResetLSB(&queens);
    }
    return score;    
}

static void PawnShields(
    DefenseScore_t* wDefense,
    DefenseScore_t* bDefense,
    const Square_t wKingSquare,
    const Square_t bKingSquare,
    BoardInfo_t* boardInfo
)
{
    const Bitboard_t wFlatShield = GetFlatPawnShield(wKingSquare, white);
    const Bitboard_t wPointedShield = GetPointedSPawnShield(wKingSquare, white);
    const Bitboard_t bFlatShield = GetFlatPawnShield(bKingSquare, black);
    const Bitboard_t bPointedShield = GetPointedSPawnShield(bKingSquare, black);

    *wDefense += flatPawnShield * ((wFlatShield & boardInfo->pawns[white]) == wFlatShield);
    *wDefense += pointedPawnShield * ((wPointedShield & boardInfo->pawns[white]) == wPointedShield);

    *bDefense += flatPawnShield * ((bFlatShield & boardInfo->pawns[black]) == bFlatShield);
    *bDefense += pointedPawnShield * ((bPointedShield & boardInfo->pawns[black]) == bPointedShield);
}

static void KingAiriness(
    DefenseScore_t* wDefense,
    DefenseScore_t* bDefense,
    const Square_t wKingSquare,
    const Square_t bKingSquare,
    BoardInfo_t* boardInfo
)
{

}

void ThreatsMobilitySafety(BoardInfo_t* boardInfo, Score_t* score) {
    const Bitboard_t wPawnAttacks = 
        NoEaOne(boardInfo->pawns[white]) | 
        NoWeOne(boardInfo->pawns[white]);
    const Bitboard_t bPawnAttacks = 
        SoEaOne(boardInfo->pawns[black]) |
        SoWeOne(boardInfo->pawns[black]);

    // MOBILITY
    // not including supporting other pieces in mobility, EVEN in x-ray attacks
    const Bitboard_t wAvailible = ~bPawnAttacks & (boardInfo->allPieces[black] | boardInfo->empty);
    const Bitboard_t bAvailible = ~wPawnAttacks & (boardInfo->allPieces[white] | boardInfo->empty);

    const Bitboard_t whiteHvEmpty = boardInfo->empty | boardInfo->rooks[white] | boardInfo->queens[white];
    const Bitboard_t whiteD12Empty = boardInfo->empty | boardInfo->bishops[white] | boardInfo->queens[white];

    const Bitboard_t blackHvEmpty = boardInfo->empty | boardInfo->rooks[black] | boardInfo->queens[black];
    const Bitboard_t blackD12Empty = boardInfo->empty | boardInfo->bishops[black] | boardInfo->queens[black];

    // KING SAFETY
    const Square_t wKingSquare = KingSquare(boardInfo, white);
    const Square_t bKingSquare = KingSquare(boardInfo, black);

    const Bitboard_t wInnerKingZone = GetKingAttackSet(wKingSquare) | boardInfo->kings[white];
    const Bitboard_t bInnerKingZone = GetKingAttackSet(bKingSquare) | boardInfo->kings[black];

    const Bitboard_t wOuterKingZone = wInnerKingZone | GenShiftNorth(wInnerKingZone, 3);
    const Bitboard_t bOuterKingZone = bInnerKingZone | GenShiftSouth(wInnerKingZone, 3);

    // COMPUTATIONS
    AttackScore_t wAttackScore = 0;
    DefenseScore_t wDefenseScore = 0;
    AttackScore_t bAttackScore = 0;
    DefenseScore_t bDefenseScore = 0;

    *score += ComputeKnights(boardInfo->knights[white], wAvailible);
    *score += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty);
    *score += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty);
    *score += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty);

    *score -= ComputeKnights(boardInfo->knights[black], bAvailible);
    *score -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty);
    *score -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty);
    *score -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty);
}