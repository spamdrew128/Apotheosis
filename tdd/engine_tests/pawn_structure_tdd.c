#include "pawn_structure_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "util_macros.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = { { PASSED_PAWN_MG_PST }, { PASSED_PAWN_EG_PST } }; 
static Centipawns_t blockerPenalty[NUM_PHASES][8] = { { BLOCKED_PASSERS_MG }, { BLOCKED_PASSERS_EG } };

static void ShouldCorrectlyEvaluatePassedPawns() {
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;

    FEN_t fen = "5k2/8/p1p4p/P4K2/8/1PP5/3PpP2/8 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    PassedPawns(&boardInfo, &mgScore, &egScore);

    Centipawns_t expectedMgBonus = passerBonus[mg_phase][MIRROR(f2)] - (passerBonus[mg_phase][e2] + passerBonus[mg_phase][h6]);
    Centipawns_t expectedEgBonus = passerBonus[eg_phase][MIRROR(f2)] - (passerBonus[eg_phase][e2] + passerBonus[eg_phase][h6]);

    PrintResults(
        mgScore == expectedMgBonus &&
        egScore == expectedEgBonus
    );
}

static void ShouldApplyBlockerPenalties() {
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;

    FEN_t fen = "8/5k2/8/3n4/3P4/1p6/1N6/3K4 w - - 4 5";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    PassedPawns(&boardInfo, &mgScore, &egScore);

    Centipawns_t expectedMg = (passerBonus[mg_phase][MIRROR(d4)] - passerBonus[mg_phase][b3]) + 
        (blockerPenalty[mg_phase][MIRROR(d5) / 8] - blockerPenalty[mg_phase][b2 / 8]);

    Centipawns_t expectedEg = (passerBonus[eg_phase][MIRROR(d4)] - passerBonus[eg_phase][b3]) +
        (blockerPenalty[eg_phase][MIRROR(d5) / 8] - blockerPenalty[eg_phase][b2 / 8]);

    PrintResults(
        mgScore == expectedMg &&
        egScore == expectedEg
    );
}

void PawnStructureTDDRunner() {
    ShouldCorrectlyEvaluatePassedPawns();
    ShouldApplyBlockerPenalties();
}