#include "pawn_structure_tdd.h"
#include "debug.h"
#include "FEN.h"

static void ShouldCorrectlyEvaluatePassedPawns() {
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;

    FEN_t fen = "5k2/8/p1p4p/P4K2/8/1PP5/3PpP2/8 w - - 0 1";
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    Centipawns_t bonus[] = { PASSED_PAWN_BONUS };
    PassedPawns(&boardInfo, &mgScore, &egScore);

    int expected = -1; // white has 1 passer, black has 2

    PrintResults(
        mgScore == bonus[mg_phase]*expected &&
        egScore == bonus[eg_phase]*expected
    );
}

void PawnStructureTDDRunner() {
    ShouldCorrectlyEvaluatePassedPawns();
}