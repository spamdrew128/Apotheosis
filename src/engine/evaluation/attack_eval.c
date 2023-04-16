#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static void UpdateAttackInfo(AttackInfo_t* attackInfo, const Bitboard_t moves, const AttackScore_t attackValue, const int weight) {
    const Bitboard_t attacks = moves & attackInfo->attackZone;
    const Bitboard_t innerAttacks = moves & attackInfo->innerKingRing;

    attackInfo->attackScore += PopCount(attacks) * attackValue + PopCount(innerAttacks) * inner_ring_bonus;
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
        ResetLSB(&queens);
    }
    return score;    
}

static void ComputePawns(
    Bitboard_t pawnAttacks,
    AttackInfo_t* attackInfo
)
{
    const Bitboard_t innerAttacks = pawnAttacks & attackInfo->innerKingRing;

    attackInfo->attackScore += PopCount(innerAttacks) * inner_ring_bonus;
    attackInfo->attackerCount += (bool)innerAttacks;
}

static void ComputeKing(
    Bitboard_t kingAttacks,
    AttackInfo_t* attackInfo
)
{
    const Bitboard_t innerAttacks = kingAttacks & attackInfo->innerKingRing;

    attackInfo->attackScore += PopCount(innerAttacks) * inner_ring_bonus;
    attackInfo->attackerCount += (bool)innerAttacks;
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
        .innerKingRing = GetKingAttackSet(KingSquare(boardInfo, black)),
    };

    AttackInfo_t blackAttack = {
        .attackerCount = 0,
        .attackScore = 0,
        .attackZone = GetVulnerableKingZone(KingSquare(boardInfo, white), white),
        .innerKingRing = GetKingAttackSet(KingSquare(boardInfo, white)),
    };

    *score += ComputeKnights(boardInfo->knights[white], wAvailible, &whiteAttack);
    *score += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty, &whiteAttack);
    *score += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty, &whiteAttack);
    *score += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, &whiteAttack);
    ComputePawns(wPawnAttacks, &whiteAttack);
    ComputeKing(wKingAttacks, &whiteAttack);

    *score -= ComputeKnights(boardInfo->knights[black], bAvailible, &blackAttack);
    *score -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty, &blackAttack);
    *score -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty, &blackAttack);
    *score -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, &blackAttack);
    ComputePawns(bPawnAttacks, &blackAttack);
    ComputeKing(bKingAttacks, &blackAttack);


    if(whiteAttack.attackerCount > 2) {
        *score += whiteAttack.attackScore;
    }
    if(blackAttack.attackerCount > 2) {
        *score += blackAttack.attackScore;
    }
}

void TDDMobilityStuff(BoardInfo_t* boardInfo, AttackInfo_t* wAttack, AttackInfo_t* bAttack, Score_t* nonSafetyScore) {
    *nonSafetyScore = 0;

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
        .innerKingRing = GetKingAttackSet(KingSquare(boardInfo, black)),
    };

    AttackInfo_t blackAttack = {
        .attackerCount = 0,
        .attackScore = 0,
        .attackZone = GetVulnerableKingZone(KingSquare(boardInfo, white), white),
        .innerKingRing = GetKingAttackSet(KingSquare(boardInfo, white)),
    };

    *nonSafetyScore += ComputeKnights(boardInfo->knights[white], wAvailible, &whiteAttack);
    *nonSafetyScore += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty, &whiteAttack);
    *nonSafetyScore += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty, &whiteAttack);
    *nonSafetyScore += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, &whiteAttack);
    ComputePawns(wPawnAttacks, &whiteAttack);
    ComputeKing(wKingAttacks, &whiteAttack);

    *nonSafetyScore -= ComputeKnights(boardInfo->knights[black], bAvailible, &blackAttack);
    *nonSafetyScore -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty, &blackAttack);
    *nonSafetyScore -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty, &blackAttack);
    *nonSafetyScore -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, &blackAttack);
    ComputePawns(bPawnAttacks, &blackAttack);
    ComputeKing(bKingAttacks, &blackAttack);

    *wAttack = whiteAttack;
    *bAttack = blackAttack;
}