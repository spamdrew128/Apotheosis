#include "pawn_structure_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "util_macros.h"

static void ShouldCorrectlyEvaluatePassedPawns() {
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;

    FEN_t fen = "5k2/8/p1p4p/P4K2/8/1PP5/3PpP2/8 w - - 0 1";
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = { { PASSED_PAWN_MG_PST }, { PASSED_PAWN_EG_PST } }; 
    PassedPawns(&boardInfo, &mgScore, &egScore);

    Centipawns_t expectedMgBonus = passerBonus[mg_phase][MIRROR(f2)] - (passerBonus[mg_phase][e2] + passerBonus[mg_phase][h6]);
    Centipawns_t expectedEgBonus = passerBonus[eg_phase][MIRROR(f2)] - (passerBonus[eg_phase][e2] + passerBonus[eg_phase][h6]);

    PrintResults(
        mgScore == expectedMgBonus &&
        egScore == expectedEgBonus
    );
}

void PawnStructureTDDRunner() {
    ShouldCorrectlyEvaluatePassedPawns();
}