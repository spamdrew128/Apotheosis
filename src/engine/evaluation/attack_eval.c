#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static Score_t kingRingThreats[NUM_PIECES - 1] = KING_RING_THREATS;

static Score_t ComputeKnights(
    Bitboard_t knights,
    Bitboard_t availible,
    const Bitboard_t enemyBishops,
    const Bitboard_t enemyRooks,
    const Bitboard_t enemyQueens,
    const Bitboard_t enemyKingRing
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

        score += kingRingThreats[knight] * PopCount(attacks & enemyKingRing);
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
    const Bitboard_t enemyKingRing
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

        score += kingRingThreats[bishop] * PopCount(attacks & enemyKingRing);
        ResetLSB(&bishops);
    }
    return score;
}

static Score_t ComputeRooks(
    Bitboard_t rooks,
    Bitboard_t availible,
    const Bitboard_t hvEmpty,
    const Bitboard_t enemyQueens,
    const Bitboard_t enemyKingRing
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

        score += kingRingThreats[rook] * PopCount(attacks & enemyKingRing);
        ResetLSB(&rooks);
    }
    return score;    
}

static Score_t ComputeQueens(
    Bitboard_t queens,
    Bitboard_t availible,
    const Bitboard_t hvEmpty,
    const Bitboard_t d12Empty,
    const Bitboard_t enemyKingRing
)
{
    Score_t score = 0;
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t attacks = GetBishopAttackSet(sq, d12Empty) | GetRookAttackSet(sq, hvEmpty);
        Bitboard_t moves = attacks & availible;
        score += queenMobility[PopCount(moves)];

        score += kingRingThreats[queen] * PopCount(attacks & enemyKingRing);
        ResetLSB(&queens);
    }
    return score;    
}

static Score_t PawnThreats(
    BoardInfo_t* boardInfo,
    const Bitboard_t wPawnAttacks,
    const Bitboard_t bPawnAttacks,
    const Bitboard_t wKingRing,
    const Bitboard_t bKingRing
)
{
    int knightThreats = PopCount(wPawnAttacks & boardInfo->knights[black]) - PopCount(bPawnAttacks & boardInfo->knights[white]);
    int bishopThreats = PopCount(wPawnAttacks & boardInfo->bishops[black]) - PopCount(bPawnAttacks & boardInfo->bishops[white]);
    int rookThreats = PopCount(wPawnAttacks & boardInfo->rooks[black]) - PopCount(bPawnAttacks & boardInfo->rooks[white]);
    int queenThreats = PopCount(wPawnAttacks & boardInfo->queens[black]) - PopCount(bPawnAttacks & boardInfo->queens[white]);
    int pawnKingRingThreats = PopCount(wPawnAttacks & bKingRing) - PopCount(bPawnAttacks & wKingRing);
    
    return 
        knightThreats * PAWN_THREAT_ON_KNIGHT +
        bishopThreats * PAWN_THREAT_ON_BISHOP +
        rookThreats * PAWN_THREAT_ON_ROOK +
        queenThreats * PAWN_THREAT_ON_QUEEN +
        pawnKingRingThreats * kingRingThreats[pawn];
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
    const Bitboard_t wKnights = boardInfo->knights[white];
    const Bitboard_t wBishops = boardInfo->bishops[white];
    const Bitboard_t wRooks = boardInfo->rooks[white];
    const Bitboard_t wQueens = boardInfo->queens[white];

    const Bitboard_t bKnights = boardInfo->knights[black];
    const Bitboard_t bBishops = boardInfo->bishops[black];
    const Bitboard_t bRooks = boardInfo->rooks[black];
    const Bitboard_t bQueens = boardInfo->queens[black];

    // KING SAFETY
    const Bitboard_t wKingRing = GetKingAttackSet(KingSquare(boardInfo, white));
    const Bitboard_t bKingRing = GetKingAttackSet(KingSquare(boardInfo, black));

    // COMPUTATIONS
    *score += ComputeKnights(boardInfo->knights[white], wAvailible, bBishops, bRooks, bQueens, bKingRing);
    *score += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty, bKnights, bRooks, bQueens, bKingRing);
    *score += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty, bQueens, bKingRing);
    *score += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, bKingRing);

    *score -= ComputeKnights(boardInfo->knights[black], bAvailible, wBishops, wRooks, wQueens, wKingRing);
    *score -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty, wKnights, wRooks, wQueens, wKingRing);
    *score -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty, wQueens, wKingRing);
    *score -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, wKingRing);

    *score += PawnThreats(boardInfo, wPawnAttacks, bPawnAttacks, wKingRing, bKingRing);
}