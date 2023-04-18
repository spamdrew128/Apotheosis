#include "attack_eval_tdd.h"
#include "debug.h"
#include "eval_constants.h"
#include "zobrist.h"
#include "FEN.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static Score_t kingRingThreats[NUM_PIECES - 1] = KING_RING_THREATS;

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

    Score_t safetyScore = 
        (kingRingThreats[pawn] + 2 * kingRingThreats[bishop]) -
        (kingRingThreats[bishop] + kingRingThreats[queen]);

    PrintResults(score == (queenScore + rookScore + bishopScore + knightScore + threatScore + safetyScore));
}

void AttackEvalTDDRunner() {
    ShouldCalculateMobility();
}