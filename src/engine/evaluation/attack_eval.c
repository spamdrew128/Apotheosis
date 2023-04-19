#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static Score_t checkBonus[NUM_PIECES - 1] = CHECK_BONUS;

static Score_t knightKZThreats[KING_SAFETY_ZONE_SQUARES] = KNIGHT_KING_ZONE_ATTACKS;
static Score_t bishopKZThreats[KING_SAFETY_ZONE_SQUARES] = BISHOP_KING_ZONE_ATTACKS;
static Score_t rookKZThreats[KING_SAFETY_ZONE_SQUARES] = ROOK_KING_ZONE_ATTACKS;
static Score_t queenKZThreats[KING_SAFETY_ZONE_SQUARES] = QUEEN_KING_ZONE_ATTACKS;
static Score_t pawnKZThreats[KING_SAFETY_ZONE_SQUARES] = PAWN_KING_ZONE_ATTACKS;

static Score_t ComputeKnights(
    Bitboard_t knights,
    Bitboard_t availible,
    const Bitboard_t enemyBishops,
    const Bitboard_t enemyRooks,
    const Bitboard_t enemyQueens,
    const Bitboard_t enemyKingZone,
    const Bitboard_t enemyKing
)
{
    Score_t score = 0;
    while(knights) {
        Square_t sq = LSB(knights);
        Bitboard_t attacks = GetKnightAttackSet(sq);
        Bitboard_t moves = attacks & availible;
        score += knightMobility[PopCount(moves)];

        score += 
            KNIGHT_THREAT_ON_BISHOP * PopCount(attacks & enemyBishops) +
            KNIGHT_THREAT_ON_ROOK * PopCount(attacks & enemyRooks) +
            KNIGHT_THREAT_ON_QUEEN * PopCount(attacks & enemyQueens);

        score += knightKZThreats[PopCount(attacks & enemyKingZone)];
        score += checkBonus[knight] * PopCount(attacks & enemyKing);
        ResetLSB(&knights);
    }
    return score;
}

static Score_t ComputeBishops(
    Bitboard_t bishops,
    Bitboard_t availible,
    const Bitboard_t d12Empty,
    const Bitboard_t enemyKnights,
    const Bitboard_t enemyRooks,
    const Bitboard_t enemyQueens,
    const Bitboard_t enemyKingZone,
    const Bitboard_t enemyKing
)
{
    Score_t score = 0;
    while(bishops) {
        Square_t sq = LSB(bishops);
        Bitboard_t attacks = GetBishopAttackSet(sq, d12Empty);
        Bitboard_t moves = attacks & availible;
        score += bishopMobility[PopCount(moves)];

        score += 
            BISHOP_THREAT_ON_KNIGHT * PopCount(attacks & enemyKnights) +
            BISHOP_THREAT_ON_ROOK * PopCount(attacks & enemyRooks) +
            BISHOP_THREAT_ON_QUEEN * PopCount(attacks & enemyQueens);

        score += bishopKZThreats[PopCount(attacks & enemyKingZone)];
        score += checkBonus[bishop] * PopCount(attacks & enemyKing);
        ResetLSB(&bishops);
    }
    return score;
}

static Score_t ComputeRooks(
    Bitboard_t rooks,
    Bitboard_t availible,
    const Bitboard_t hvEmpty,
    const Bitboard_t enemyQueens,
    const Bitboard_t enemyKingZone,
    const Bitboard_t enemyKing
)
{
    Score_t score = 0;
    while(rooks) {
        Square_t sq = LSB(rooks);
        Bitboard_t attacks = GetRookAttackSet(sq, hvEmpty);
        Bitboard_t moves = attacks & availible;
        score += rookMobility[PopCount(moves)];

        score += 
            ROOK_THREAT_ON_QUEEN * PopCount(attacks & enemyQueens);

        score += rookKZThreats[PopCount(attacks & enemyKingZone)];
        score += checkBonus[rook] * PopCount(attacks & enemyKing);
        ResetLSB(&rooks);
    }
    return score;    
}

static Score_t ComputeQueens(
    Bitboard_t queens,
    Bitboard_t availible,
    const Bitboard_t hvEmpty,
    const Bitboard_t d12Empty,
    const Bitboard_t enemyKingZone,
    const Bitboard_t enemyKing
)
{
    Score_t score = 0;
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t attacks = GetBishopAttackSet(sq, d12Empty) | GetRookAttackSet(sq, hvEmpty);
        Bitboard_t moves = attacks & availible;
        score += queenMobility[PopCount(moves)];

        score += queenKZThreats[PopCount(attacks & enemyKingZone)];
        score += checkBonus[queen] * PopCount(attacks & enemyKing);
        ResetLSB(&queens);
    }
    return score;    
}

static Score_t PawnThreats(
    BoardInfo_t* boardInfo,
    const Bitboard_t wPawnAttacks,
    const Bitboard_t bPawnAttacks,
    const Bitboard_t wKingZone,
    const Bitboard_t bKingZone,
    const Bitboard_t wKing,
    const Bitboard_t bKing
)
{
    int knightThreats = PopCount(wPawnAttacks & boardInfo->knights[black]) - PopCount(bPawnAttacks & boardInfo->knights[white]);
    int bishopThreats = PopCount(wPawnAttacks & boardInfo->bishops[black]) - PopCount(bPawnAttacks & boardInfo->bishops[white]);
    int rookThreats = PopCount(wPawnAttacks & boardInfo->rooks[black]) - PopCount(bPawnAttacks & boardInfo->rooks[white]);
    int queenThreats = PopCount(wPawnAttacks & boardInfo->queens[black]) - PopCount(bPawnAttacks & boardInfo->queens[white]);
    int pawnChecks = PopCount(wPawnAttacks & bKing) - PopCount(bPawnAttacks & wKing);

    return 
        knightThreats * PAWN_THREAT_ON_KNIGHT +
        bishopThreats * PAWN_THREAT_ON_BISHOP +
        rookThreats * PAWN_THREAT_ON_ROOK +
        queenThreats * PAWN_THREAT_ON_QUEEN +

        pawnKZThreats[PopCount(wPawnAttacks & bKingZone)] - pawnKZThreats[PopCount(bPawnAttacks & wKingZone)] +
        pawnChecks * checkBonus[pawn];
}

void MobilityAndThreatsEval(BoardInfo_t* boardInfo, Score_t* score) {
    const Bitboard_t wPawnAttacks = 
        NoEaOne(boardInfo->pawns[white]) | 
        NoWeOne(boardInfo->pawns[white]);
    const Bitboard_t bPawnAttacks = 
        SoEaOne(boardInfo->pawns[black]) |
        SoWeOne(boardInfo->pawns[black]);

    // MOBILITY STUFF
    // not including supporting other pieces in mobility, EVEN in x-ray attacks
    const Bitboard_t wAvailible = ~bPawnAttacks & (boardInfo->allPieces[black] | boardInfo->empty);
    const Bitboard_t bAvailible = ~wPawnAttacks & (boardInfo->allPieces[white] | boardInfo->empty);

    const Bitboard_t whiteHvEmpty = boardInfo->empty | boardInfo->rooks[white] | boardInfo->queens[white];
    const Bitboard_t whiteD12Empty = boardInfo->empty | boardInfo->bishops[white] | boardInfo->queens[white];

    const Bitboard_t blackHvEmpty = boardInfo->empty | boardInfo->rooks[black] | boardInfo->queens[black];
    const Bitboard_t blackD12Empty = boardInfo->empty | boardInfo->bishops[black] | boardInfo->queens[black];

    // THREAT STUFF
    const Bitboard_t wKing = boardInfo->kings[white];
    const Bitboard_t wKnights = boardInfo->knights[white];
    const Bitboard_t wBishops = boardInfo->bishops[white];
    const Bitboard_t wRooks = boardInfo->rooks[white];
    const Bitboard_t wQueens = boardInfo->queens[white];

    const Bitboard_t bKing = boardInfo->kings[black];
    const Bitboard_t bKnights = boardInfo->knights[black];
    const Bitboard_t bBishops = boardInfo->bishops[black];
    const Bitboard_t bRooks = boardInfo->rooks[black];
    const Bitboard_t bQueens = boardInfo->queens[black];

    // KING SAFETY
    const Bitboard_t wKingZone = GetKingSafetyZone(KingSquare(boardInfo, white), white);
    const Bitboard_t bKingZone = GetKingSafetyZone(KingSquare(boardInfo, black), black);

    // COMPUTATIONS
    *score += ComputeKnights(wKnights, wAvailible, bBishops, bRooks, bQueens, bKingZone, bKing);
    *score += ComputeBishops(wBishops, wAvailible, whiteD12Empty, bKnights, bRooks, bQueens, bKingZone, bKing);
    *score += ComputeRooks(wRooks, wAvailible, whiteHvEmpty, bQueens, bKingZone, bKing);
    *score += ComputeQueens(wQueens, wAvailible, whiteHvEmpty, whiteD12Empty, bKingZone, bKing);

    *score -= ComputeKnights(bKnights, bAvailible, wBishops, wRooks, wQueens, wKingZone, wKing);
    *score -= ComputeBishops(bBishops, bAvailible, blackD12Empty, wKnights, wRooks, wQueens, wKingZone, wKing);
    *score -= ComputeRooks(bRooks, bAvailible, blackHvEmpty, wQueens, wKingZone, wKing);
    *score -= ComputeQueens(bQueens, bAvailible, blackHvEmpty, blackD12Empty, wKingZone, wKing);

    *score += PawnThreats(boardInfo, wPawnAttacks, bPawnAttacks, wKingZone, bKingZone, wKing, bKing);
}