#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static void UpdateAttackInfo(AttackInfo_t* attackInfo, const Bitboard_t moves, const AttackScore_t attackValue, const int weight) {
    const Bitboard_t attacks = moves & attackInfo->attackZone;

    attackInfo->attackScore += PopCount(attacks) * attackValue;
    attackInfo->attackerCount += weight * (bool)attacks;
}

static Score_t ComputeKnights(
    Bitboard_t knights,
    Bitboard_t availible,
    AttackInfo_t* attackInfo
)
{
    Score_t score = 0;
    while(knights) {
        Square_t sq = LSB(knights);
        Bitboard_t attacks = GetKnightAttackSet(sq);
        Bitboard_t moves = attacks & availible;
        score += knightMobility[PopCount(moves)];

        UpdateAttackInfo(attackInfo, moves, minor_attack, other_piece_weight);
        attackInfo->pawnKnightControl |= attacks;
        ResetLSB(&knights);
    }
    return score;
}

static Score_t ComputeBishops(
    Bitboard_t bishops,
    Bitboard_t availible,
    Bitboard_t d12Empty,
    AttackInfo_t* attackInfo
)
{
    Score_t score = 0;
    while(bishops) {
        Square_t sq = LSB(bishops);
        Bitboard_t attacks = GetBishopAttackSet(sq, d12Empty);
        Bitboard_t moves = attacks & availible;
        score += bishopMobility[PopCount(moves)];

        UpdateAttackInfo(attackInfo, moves, minor_attack, other_piece_weight);
        attackInfo->sliderControl |= attacks;
        ResetLSB(&bishops);
    }
    return score;
}

static Score_t ComputeRooks(
    Bitboard_t rooks, 
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    AttackInfo_t* attackInfo
)
{
    Score_t score = 0;
    while(rooks) {
        Square_t sq = LSB(rooks);
        Bitboard_t attacks = GetRookAttackSet(sq, hvEmpty);
        Bitboard_t moves = attacks & availible;
        score += rookMobility[PopCount(moves)];

        UpdateAttackInfo(attackInfo, moves, rook_attack, other_piece_weight);
        attackInfo->sliderControl |= attacks;
        ResetLSB(&rooks);
    }
    return score;    
}

static Score_t ComputeQueens(
    Bitboard_t queens,
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    Bitboard_t d12Empty,
    AttackInfo_t* attackInfo
)
{
    Score_t score = 0;
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t attacks = GetBishopAttackSet(sq, d12Empty) | GetRookAttackSet(sq, hvEmpty);
        Bitboard_t moves = attacks & availible;
        score += queenMobility[PopCount(moves)];

        UpdateAttackInfo(attackInfo, moves, queen_attack, queen_piece_weight);
        attackInfo->sliderControl |= attacks;
        ResetLSB(&queens);
    }
    return score;    
}

void MobilitySafetyThreatsEval(BoardInfo_t* boardInfo, Score_t* score) {
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
    const Bitboard_t wKingAttacks = GetKingAttackSet(boardInfo->kings[white]);
    const Bitboard_t bKingAttacks = GetKingAttackSet(boardInfo->kings[black]);

    AttackInfo_t whiteAttack = {
        .attackerCount = 0,
        .attackScore = 0,
        .attackZone = GetVulnerableKingZone(KingSquare(boardInfo, black), black),

        .rookContactRing = GetRookContactCheckZone(KingSquare(boardInfo, black)),
        .queenContactRing = bKingAttacks,

        // these will be filled out fully during computations
        .pawnKnightControl = wKingAttacks | wPawnAttacks,
        .sliderControl = empty_set,
    };

    AttackInfo_t blackAttack = {
        .attackerCount = 0,
        .attackScore = 0,
        .attackZone = GetVulnerableKingZone(KingSquare(boardInfo, white), white),

        .rookContactRing = GetRookContactCheckZone(KingSquare(boardInfo, white)),
        .queenContactRing = wKingAttacks,

        // these will be filled out fully during computations
        .pawnKnightControl = bKingAttacks | bPawnAttacks,
        .sliderControl = empty_set,
    };

    *score += ComputeKnights(boardInfo->knights[white], wAvailible, &whiteAttack);
    *score += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty, &whiteAttack);
    *score += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty, &whiteAttack);
    *score += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, &whiteAttack);

    *score -= ComputeKnights(boardInfo->knights[black], bAvailible, &blackAttack);
    *score -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty, &blackAttack);
    *score -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty, &blackAttack);
    *score -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, &blackAttack);

    if(whiteAttack.attackerCount > 2) {

    }
    if(blackAttack.attackerCount > 2) {

    }
}

void TDDSafetyOnly(BoardInfo_t* boardInfo, AttackInfo_t* wAttack, AttackInfo_t* bAttack) {
    Score_t score = 0;

    // *wAttack = whiteAttack;
    // *bAttack = blackAttack;
}