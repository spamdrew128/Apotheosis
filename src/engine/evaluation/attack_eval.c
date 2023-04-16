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
    attackInfo->allAttacks |= moves;
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
        Bitboard_t moves = GetKnightAttackSet(sq) & availible;
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
        Bitboard_t moves = GetBishopAttackSet(sq, d12Empty) & availible;
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
        Bitboard_t moves = GetRookAttackSet(sq, hvEmpty) & availible;
        score += rookMobility[PopCount(moves)];

        UpdateAttackInfo(attackInfo, moves, rook_attack, other_piece_weight);
        attackInfo->rookContacts |= moves & attackInfo->rookContactRing;
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
        Bitboard_t moves = (GetBishopAttackSet(sq, d12Empty) | GetRookAttackSet(sq, hvEmpty)) & availible;
        score += queenMobility[PopCount(moves)];

        UpdateAttackInfo(attackInfo, moves, queen_attack, queen_piece_weight);
        attackInfo->queenContacts |= moves & attackInfo->queenContactRing;
        ResetLSB(&queens);
    }
    return score;    
}

static void SafeContactChecks(
    BoardInfo_t* boardInfo,
    AttackInfo_t* attackInfo,
    const Bitboard_t friendlyPawnAttacks,
    const Bitboard_t enemyAttacked,
    const Color_t color
)
{
    const Bitboard_t allSupport = boardInfo->allPieces[white] & ~boardInfo->pawns[white];

    Bitboard_t rooksChecks = attackInfo->rookContacts & ~enemyAttacked;
    Bitboard_t queenChecks = attackInfo->queenContacts & ~enemyAttacked;

    while(rooksChecks) {
        Square_t sq = LSB(rooksChecks);
        const Bitboard_t bitset = GetSingleBitset(sq);

        const Bitboard_t empty = boardInfo->empty | bitset; // we don't include ourself
        const Bitboard_t support = allSupport & ~bitset;

        const Bitboard_t supportPaths = 
            GetBishopAttackSet(sq, empty) | GetRookAttackSet(sq, empty) | GetKnightAttackSet(sq);
            
        if((bitset & friendlyPawnAttacks) || (supportPaths & support)) {
            attackInfo->attackScore += rook_contact_check;
            break;
        }

        ResetLSB(&rooksChecks);
    }

    while(queenChecks) {
        Square_t sq = LSB(queenChecks);
        const Bitboard_t bitset = GetSingleBitset(sq);

        const Bitboard_t empty = boardInfo->empty | bitset;
        const Bitboard_t support = allSupport & ~bitset;

        const Bitboard_t supportPaths = 
            GetBishopAttackSet(sq, empty) | GetRookAttackSet(sq, empty) | GetKnightAttackSet(sq);
            
        if((bitset & friendlyPawnAttacks) || (supportPaths & support)) {
            attackInfo->attackScore += queen_contact_check;
            break;
        }

        ResetLSB(&queenChecks);
    }
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
        .rookContacts = empty_set,
        .queenContacts = empty_set,

        .allAttacks = wPawnAttacks | (wKingAttacks & wAvailible),
    };

    AttackInfo_t blackAttack = {
        .attackerCount = 0,
        .attackScore = 0,
        .attackZone = GetVulnerableKingZone(KingSquare(boardInfo, white), white),

        .rookContactRing = GetRookContactCheckZone(KingSquare(boardInfo, white)),
        .queenContactRing = wKingAttacks,
        .rookContacts = empty_set,
        .queenContacts = empty_set,

        .allAttacks = bPawnAttacks | (bKingAttacks & bAvailible),
    };

    *score += ComputeKnights(boardInfo->knights[white], wAvailible, &whiteAttack);
    *score += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty, &whiteAttack);
    *score += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty, &whiteAttack);
    *score += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, &whiteAttack);

    *score -= ComputeKnights(boardInfo->knights[black], bAvailible, &blackAttack);
    *score -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty, &blackAttack);
    *score -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty, &blackAttack);
    *score -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, &blackAttack);

    SafeContactChecks(boardInfo, &whiteAttack, wPawnAttacks, blackAttack.allAttacks, white);
    SafeContactChecks(boardInfo, &blackAttack, bPawnAttacks, whiteAttack.allAttacks, black);
}