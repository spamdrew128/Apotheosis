#include <stdint.h>

#include "attack_eval.h"
#include "lookup.h"
#include "bitboards.h"
#include "util_macros.h"

static const Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static const Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static const Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static const Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

typedef Score_t AttackScore_t;
typedef Score_t DefenseScore_t;
static const AttackScore_t innerAttacks[NUM_PIECES] = INNER_ATTACKS;
static const AttackScore_t outerAttacks[NUM_PIECES] = OUTER_ATTACKS;

static const DefenseScore_t innerDefense[NUM_PIECES-1] = INNER_DEFENSE; // kings don't defend themselves so only 5 indexes
static const DefenseScore_t outerDefense[NUM_PIECES-1] = OUTER_DEFENSE; 
static const DefenseScore_t kingAiriness[KING_AIRINESS_OPTIONS] = KING_AIRINESS;
static const DefenseScore_t kingSafetyPST[NUM_SQUARES] = KING_SAFETY_PST;

static void UpdateSafety(
    const Bitboard_t moves,
    const Piece_t piece,
    const Bitboard_t friendlyInnerKing,
    const Bitboard_t friendlyOuterKing,
    const Bitboard_t enemyInnerKing,
    const Bitboard_t enemyOuterKing,
    AttackScore_t* attackScore,
    DefenseScore_t* defenseScore
)
{
    *attackScore += 
        PopCount(moves & enemyInnerKing) * innerAttacks[piece] +
        PopCount(moves & enemyOuterKing) * outerAttacks[piece];
        
    *defenseScore += 
        PopCount(moves & friendlyInnerKing) * innerDefense[piece] +
        PopCount(moves & friendlyOuterKing) * outerDefense[piece];
}

static Score_t ComputeKnights(
    Bitboard_t knights,
    Bitboard_t availible,
    const Bitboard_t friendlyInnerKing,
    const Bitboard_t friendlyOuterKing,
    const Bitboard_t enemyInnerKing,
    const Bitboard_t enemyOuterKing,
    AttackScore_t* attackScore,
    DefenseScore_t* defenseScore
)
{
    Score_t score = 0;
    while(knights) {
        Square_t sq = LSB(knights);
        // MOBILITY
        Bitboard_t moves = GetKnightAttackSet(sq) & availible;
        score += knightMobility[PopCount(moves)];

        // KING SAFETY
        UpdateSafety(
            moves,
            knight,
            friendlyInnerKing,
            friendlyOuterKing,
            enemyInnerKing,
            enemyOuterKing,
            attackScore,
            defenseScore
        );

        ResetLSB(&knights);
    }
    return score;
}

static Score_t ComputeBishops(
    Bitboard_t bishops,
    Bitboard_t availible,
    Bitboard_t d12Empty,
    const Bitboard_t friendlyInnerKing,
    const Bitboard_t friendlyOuterKing,
    const Bitboard_t enemyInnerKing,
    const Bitboard_t enemyOuterKing,
    AttackScore_t* attackScore,
    DefenseScore_t* defenseScore
)
{
    Score_t score = 0;
    while(bishops) {
        Square_t sq = LSB(bishops);
        // MOBILITY
        Bitboard_t moves = GetBishopAttackSet(sq, d12Empty) & availible;
        score += bishopMobility[PopCount(moves)];

        // KING SAFETY
        UpdateSafety(
            moves,
            bishop,
            friendlyInnerKing,
            friendlyOuterKing,
            enemyInnerKing,
            enemyOuterKing,
            attackScore,
            defenseScore
        );

        ResetLSB(&bishops);
    }
    return score;
}

static Score_t ComputeRooks(
    Bitboard_t rooks, 
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    const Bitboard_t friendlyInnerKing,
    const Bitboard_t friendlyOuterKing,
    const Bitboard_t enemyInnerKing,
    const Bitboard_t enemyOuterKing,
    AttackScore_t* attackScore,
    DefenseScore_t* defenseScore
)
{
    Score_t score = 0;
    while(rooks) {
        Square_t sq = LSB(rooks);
        // MOBILITY
        Bitboard_t moves = GetRookAttackSet(sq, hvEmpty) & availible;
        score += rookMobility[PopCount(moves)];

        // KING SAFETY
        UpdateSafety(
            moves,
            rook,
            friendlyInnerKing,
            friendlyOuterKing,
            enemyInnerKing,
            enemyOuterKing,
            attackScore,
            defenseScore
        );

        ResetLSB(&rooks);
    }
    return score;    
}

static Score_t ComputeQueens(
    Bitboard_t queens,
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    Bitboard_t d12Empty,
    const Bitboard_t friendlyInnerKing,
    const Bitboard_t friendlyOuterKing,
    const Bitboard_t enemyInnerKing,
    const Bitboard_t enemyOuterKing,
    AttackScore_t* attackScore,
    DefenseScore_t* defenseScore
)
{
    Score_t score = 0;
    while(queens) {
        Square_t sq = LSB(queens);
        // MOBILITY
        Bitboard_t d12Moves = GetBishopAttackSet(sq, d12Empty) & availible;
        Bitboard_t hvMoves = GetRookAttackSet(sq, hvEmpty) & availible;
        Bitboard_t moves = hvMoves | d12Moves;
        score += queenMobility[PopCount(moves)];

        // KING SAFETY
        UpdateSafety(
            moves,
            queen,
            friendlyInnerKing,
            friendlyOuterKing,
            enemyInnerKing,
            enemyOuterKing,
            attackScore,
            defenseScore
        );

        ResetLSB(&queens);
    }
    return score;    
}

static void KingAiriness(
    DefenseScore_t* wDefense,
    DefenseScore_t* bDefense,
    const Square_t wKingSquare,
    const Square_t bKingSquare,
    BoardInfo_t* boardInfo
)
{
    const Bitboard_t whiteMobilePieces = boardInfo->allPieces[white] ^ boardInfo->pawns[white];
    const Bitboard_t blackMobilePieces = boardInfo->allPieces[black] ^ boardInfo->pawns[black];

    // we want to count mobile enemy pieces as empty for king airiness. If an enemy knight is right next to us and
    // blocking a file, that doesn't mean we are safer than we would be without it there because it can move!
    const Bitboard_t wVirtualEmpty = boardInfo->empty | blackMobilePieces;
    const Bitboard_t bVirtualEmpty = boardInfo->empty | whiteMobilePieces;

    const Bitboard_t whiteAiriness = GetRookAttackSet(wKingSquare, wVirtualEmpty) | GetBishopAttackSet(wKingSquare, wVirtualEmpty);
    const Bitboard_t blackAiriness = GetRookAttackSet(bKingSquare, bVirtualEmpty) | GetBishopAttackSet(bKingSquare, bVirtualEmpty);

    *wDefense += kingAiriness[PopCount(whiteAiriness)];
    *bDefense += kingAiriness[PopCount(blackAiriness)];
}

static void KingAttackContribution(
    AttackScore_t* wAttack,
    AttackScore_t* bAttack,
    const Bitboard_t wKingAttacks,
    const Bitboard_t bKingAttacks,
    const Bitboard_t wInnerKing,
    const Bitboard_t wOuterKing,
    const Bitboard_t bInnerKing,
    const Bitboard_t bOuterKing
)
{
    *wAttack += PopCount(wKingAttacks & bInnerKing) * innerAttacks[king];
    *wAttack += PopCount(wKingAttacks & bOuterKing) * outerAttacks[king];

    *bAttack += PopCount(bKingAttacks & wInnerKing) * innerAttacks[king];
    *bAttack += PopCount(bKingAttacks & wOuterKing) * outerAttacks[king];
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

    const Bitboard_t wOuterKingZone = GetOuterKingZone(wKingSquare, white);
    const Bitboard_t bOuterKingZone = GetOuterKingZone(bKingSquare, black);

    const Bitboard_t wKingAttacks = GetKingAttackSet(wKingSquare) & wAvailible;
    const Bitboard_t bKingAttacks = GetKingAttackSet(bKingSquare) & bAvailible;

    // COMPUTATIONS
    AttackScore_t wAttackScore = 0;
    DefenseScore_t wDefenseScore = 0;
    AttackScore_t bAttackScore = 0;
    DefenseScore_t bDefenseScore = 0;

    wDefenseScore += kingSafetyPST[MIRROR(wKingSquare)];
    bDefenseScore += kingSafetyPST[bKingSquare];
    KingAiriness(&wDefenseScore, &bDefenseScore, wKingSquare, bKingSquare, boardInfo);
    KingAttackContribution(&wAttackScore, &bAttackScore, wKingAttacks, bKingAttacks, wInnerKingZone, wOuterKingZone, bInnerKingZone, bOuterKingZone);

    *score += ComputeKnights(boardInfo->knights[white], wAvailible, wInnerKingZone, wOuterKingZone, bInnerKingZone, bOuterKingZone, &wAttackScore, &wDefenseScore);
    *score += ComputeBishops(boardInfo->bishops[white], wAvailible, whiteD12Empty, wInnerKingZone, wOuterKingZone, bInnerKingZone, bOuterKingZone, &wAttackScore, &wDefenseScore);
    *score += ComputeRooks(boardInfo->rooks[white], wAvailible, whiteHvEmpty, wInnerKingZone, wOuterKingZone, bInnerKingZone, bOuterKingZone, &wAttackScore, &wDefenseScore);
    *score += ComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, wInnerKingZone, wOuterKingZone, bInnerKingZone, bOuterKingZone, &wAttackScore, &wDefenseScore);
    UpdateSafety(wPawnAttacks, pawn, wInnerKingZone, wOuterKingZone, bInnerKingZone, bOuterKingZone, &wAttackScore, &wDefenseScore);

    *score -= ComputeKnights(boardInfo->knights[black], bAvailible, bInnerKingZone, bOuterKingZone, wInnerKingZone, wOuterKingZone, &bAttackScore, &bDefenseScore);
    *score -= ComputeBishops(boardInfo->bishops[black], bAvailible, blackD12Empty, bInnerKingZone, bOuterKingZone, wInnerKingZone, wOuterKingZone, &bAttackScore, &bDefenseScore);
    *score -= ComputeRooks(boardInfo->rooks[black], bAvailible, blackHvEmpty, bInnerKingZone, bOuterKingZone, wInnerKingZone, wOuterKingZone, &bAttackScore, &bDefenseScore);
    *score -= ComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, bInnerKingZone, bOuterKingZone, wInnerKingZone, wOuterKingZone, &bAttackScore, &bDefenseScore);
    UpdateSafety(bPawnAttacks, pawn, bInnerKingZone, bOuterKingZone, wInnerKingZone, wOuterKingZone, &bAttackScore, &bDefenseScore);
}