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

static Bitboard_t SliderHelpers(Bitboard_t hvSliders, Bitboard_t d12Sliders, const Bitboard_t empty) {
    Bitboard_t result = empty_set;
    while(hvSliders) {
        Square_t sq = LSB(hvSliders);
        result |= GetRookAttackSet(sq, empty);
        ResetLSB(&hvSliders);
    }
    while(d12Sliders) {
        Square_t sq = LSB(d12Sliders);
        result |= GetBishopAttackSet(sq, empty);
        ResetLSB(&d12Sliders);
    }
    return result;
}

static void SerializeRookContact(
    AttackInfo_t* attackInfo,
    Color_t color,
    BoardInfo_t* boardInfo,
    Bitboard_t contactZone,
    Bitboard_t nonSliderHelpers
) 
{
    Bitboard_t rooks = boardInfo->rooks[color];
    while(rooks) {
        Square_t sq = LSB(rooks);
        Bitboard_t contactChecks = GetRookAttackSet(sq, boardInfo->empty) & contactZone;
        if(contactChecks) {
            Bitboard_t empty = boardInfo->empty | GetSingleBitset(sq); // we don't include ourself as a helper
            Bitboard_t hvSliders = (boardInfo->rooks[color] | boardInfo->queens[color]) | ~GetSingleBitset(sq);
            Bitboard_t d12Sliders = boardInfo->bishops[color] | boardInfo->queens[color];
            Bitboard_t protected = SliderHelpers(hvSliders, d12Sliders, empty) | nonSliderHelpers;

            attackInfo->attackScore += PopCount(contactChecks & protected) * rook_contact_check;
        }
        ResetLSB(&rooks);
    }
}

static void SerializeQueenContact(
    AttackInfo_t* attackInfo,
    Color_t color,
    BoardInfo_t* boardInfo,
    Bitboard_t contactZone,
    Bitboard_t nonSliderHelpers
) 
{
    Bitboard_t queens = boardInfo->queens[color];
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t contactChecks = (GetRookAttackSet(sq, boardInfo->empty) | GetBishopAttackSet(sq, boardInfo->empty)) & contactZone;
        if(contactChecks) {
            Bitboard_t empty = boardInfo->empty | GetSingleBitset(sq); // we don't include ourself as a helper
            Bitboard_t hvSliders = (boardInfo->rooks[color] | boardInfo->queens[color]) | ~GetSingleBitset(sq);
            Bitboard_t d12Sliders = (boardInfo->bishops[color] | boardInfo->queens[color]) | ~GetSingleBitset(sq);
            Bitboard_t protected = SliderHelpers(hvSliders, d12Sliders, empty) | nonSliderHelpers;

            attackInfo->attackScore += PopCount(contactChecks & protected) * queen_contact_check;
        }
        ResetLSB(&queens);
    }
}

static void ContactChecks(BoardInfo_t* boardInfo, AttackInfo_t* whiteInfo, AttackInfo_t* blackInfo) {
    const Square_t wKingSq = KingSquare(boardInfo, white);
    const Square_t bKingSq = KingSquare(boardInfo, black);

    const Bitboard_t allWhiteAttacks = whiteInfo->pawnKnightControl | whiteInfo->sliderControl;
    const Bitboard_t allBlackAttacks = blackInfo->pawnKnightControl | blackInfo->sliderControl;

    const Bitboard_t wRookContactZone = GetRookContactCheckZone(bKingSq) & ~allBlackAttacks; // need to make sure squares are safe (ignoring king attacks because that is a givin)
    const Bitboard_t wQueenContactZone = GetKingAttackSet(bKingSq) & ~allBlackAttacks;

    const Bitboard_t bRookContactZone = GetRookContactCheckZone(wKingSq) & ~allWhiteAttacks;
    const Bitboard_t bQueenContactZone = GetKingAttackSet(wKingSq) & ~allWhiteAttacks;

    const Bitboard_t wNonSliderHelpers = whiteInfo->pawnKnightControl | GetKingAttackSet(wKingSq);
    const Bitboard_t bNonSliderHelpers = blackInfo->pawnKnightControl | GetKingAttackSet(bKingSq);

    SerializeRookContact(whiteInfo, white, boardInfo, wRookContactZone, wNonSliderHelpers);
    SerializeQueenContact(whiteInfo, white, boardInfo, bQueenContactZone, wNonSliderHelpers);

    SerializeRookContact(blackInfo, black, boardInfo, bRookContactZone, bNonSliderHelpers);
    SerializeQueenContact(blackInfo, black, boardInfo, bQueenContactZone, bNonSliderHelpers);
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


        .pawnKnightControl = wKingAttacks | wPawnAttacks,
        .sliderControl = empty_set, // these will be filled out fully during computations
    };

    AttackInfo_t blackAttack = {
        .attackerCount = 0,
        .attackScore = 0,
        .attackZone = GetVulnerableKingZone(KingSquare(boardInfo, white), white),

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

    ContactChecks(boardInfo, &whiteAttack, &blackAttack);

    // if(whiteAttack.attackerCount > 2) {

    // }
    // if(blackAttack.attackerCount > 2) {

    // }
}

void TDDSafetyOnly(BoardInfo_t* boardInfo, AttackInfo_t* wAttack, AttackInfo_t* bAttack) {
    Score_t score = 0;

    // *wAttack = whiteAttack;
    // *bAttack = blackAttack;
}