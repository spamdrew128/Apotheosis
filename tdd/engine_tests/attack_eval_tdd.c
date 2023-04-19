#include "attack_eval_tdd.h"
#include "debug.h"
#include "eval_constants.h"
#include "zobrist.h"
#include "FEN.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = KNIGHT_MOBILITY;
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = BISHOP_MOBILITY;
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = ROOK_MOBILITY;
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = QUEEN_MOBILITY;

static Score_t knightKZThreats[KING_SAFETY_ZONE_SQUARES] = KNIGHT_KING_ZONE_ATTACKS;
static Score_t bishopKZThreats[KING_SAFETY_ZONE_SQUARES] = BISHOP_KING_ZONE_ATTACKS;
static Score_t rookKZThreats[KING_SAFETY_ZONE_SQUARES] = ROOK_KING_ZONE_ATTACKS;
static Score_t queenKZThreats[KING_SAFETY_ZONE_SQUARES] = QUEEN_KING_ZONE_ATTACKS;
static Score_t pawnKZThreats[KING_SAFETY_ZONE_SQUARES] = PAWN_KING_ZONE_ATTACKS;

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
        (knightKZThreats[0] + bishopKZThreats[3] + rookKZThreats[0] + queenKZThreats[0] + pawnKZThreats[1]) -
        (knightKZThreats[0] + bishopKZThreats[2] + rookKZThreats[0] + queenKZThreats[3] + pawnKZThreats[2]);

    PrintResults(score == (queenScore + rookScore + bishopScore + knightScore + threatScore + safetyScore));
}

void AttackEvalTDDRunner() {
    ShouldCalculateMobility();
}