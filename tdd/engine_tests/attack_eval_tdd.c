#include "attack_eval_tdd.h"
#include "debug.h"
#include "eval_constants.h"
#include "zobrist.h"
#include "FEN.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

// static Score_t knightKZThreats[KING_VIRTUAL_MOBILITY_OPTIONS] = KNIGHT_KING_ZONE_ATTACKS;
static Score_t bishopKZThreats[KING_VIRTUAL_MOBILITY_OPTIONS] = BISHOP_KING_ZONE_ATTACKS;
// static Score_t rookKZThreats[KING_VIRTUAL_MOBILITY_OPTIONS] = ROOK_KING_ZONE_ATTACKS;
static Score_t queenKZThreats[KING_VIRTUAL_MOBILITY_OPTIONS] = QUEEN_KING_ZONE_ATTACKS;
static Score_t pawnKZThreats[KING_VIRTUAL_MOBILITY_OPTIONS] = PAWN_KING_ZONE_ATTACKS;

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void ShouldCalculateMobility() {
    Score_t score = 0;

    FEN_t fen = "B2r2k1/3p1p2/p4PpB/1p3b2/8/2Nq2PP/PP2R1NK/3R4 b - - 2 23";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    MobilityAndThreatsEval(&boardInfo, &score);

    Score_t queenScore = -queenMobility[13];
    Score_t rookScore = rookMobility[9] + rookMobility[9] - rookMobility[5];
    Score_t bishopScore = bishopMobility[7] + bishopMobility[4] - bishopMobility[5];
    Score_t knightScore = knightMobility[4] + knightMobility[3];

    Score_t threatScore = ROOK_THREAT_ON_QUEEN;

    VirtualMobility_t wVirtMobility = 2;
    VirtualMobility_t bVirtMobility = 5;

    Score_t safetyScore = 
        (3*bishopKZThreats[bVirtMobility] + pawnKZThreats[bVirtMobility]) -
        (2*bishopKZThreats[wVirtMobility] + 3*queenKZThreats[wVirtMobility] + 2*pawnKZThreats[wVirtMobility]);

    PrintResults(score == (queenScore + rookScore + bishopScore + knightScore + threatScore + safetyScore));
}

void AttackEvalTDDRunner() {
    ShouldCalculateMobility();
}