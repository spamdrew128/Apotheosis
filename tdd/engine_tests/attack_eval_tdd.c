#include "attack_eval_tdd.h"
#include "debug.h"
#include "eval_constants.h"
#include "zobrist.h"
#include "FEN.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;
static AttackInfo_t wAttack;
static AttackInfo_t bAttack;

static void ShouldCalculateMobility() {
    Score_t score = 0;

    FEN_t fen = "B2r2k1/3p1p2/p4PpB/1p3b2/8/2Nq2PP/PP2R1NK/3R4 b - - 2 23";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    TDDMobilityStuff(&boardInfo, &wAttack, &bAttack, &score);

    Score_t queenScore = -queenMobility[13];
    Score_t rookScore = rookMobility[9] + rookMobility[9] - rookMobility[5];
    Score_t bishopScore = bishopMobility[7] + bishopMobility[4] - bishopMobility[5];
    Score_t knightScore = knightMobility[4] + knightMobility[3];

    PrintResults(score == (queenScore + rookScore + bishopScore + knightScore));
}

static void ShouldCalculateKingSafety() {
    Score_t score = 0;

    FEN_t fen = "B2r2k1/3p1p2/p4PpB/1p3b2/8/2Nq2PP/PP2R1NK/3R4 b - - 2 23";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    TDDMobilityStuff(&boardInfo, &wAttack, &bAttack, &score);

    int wAttackerCount = 2;
    int wAttackScore = 6 * minor_attack;

    int bAttackerCount = 3;
    int bAttackScore = 1 * minor_attack + 2 * queen_attack;

    bool wCorrect = wAttack.attackerCount == wAttackerCount && wAttack.attackScore == wAttackScore; 
    bool bCorrect = bAttack.attackerCount == bAttackerCount && bAttack.attackScore == bAttackScore;
    PrintResults(wCorrect && bCorrect);
}

void AttackEvalTDDRunner() {
    ShouldCalculateMobility();
    ShouldCalculateKingSafety();
}