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

static void ShouldCalculateMobility() {
    Score_t score = 0;

    FEN_t fen = "B2r2k1/3p1p2/p4PpB/1p3b2/8/2Nq2PP/PP2R1NK/3R4 b - - 2 23";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    PawnStructure(&boardInfo, &score);

    Score_t expectedScore = 0;

    PrintResults(score == expectedScore);

}

void AttackEvalTDDRunner() {

}