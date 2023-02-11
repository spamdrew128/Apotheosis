#include "zobrist_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "board_info.h"
#include "game_state.h"

static FEN_t someFen = "1k4r1/p2n1p2/Pp1p4/3P4/1RPr3p/6P1/5P1P/R4K2 b - - 1 26";
static BoardInfo_t info;
static GameStack_t stack;

static void ShouldGenerateNonZeroHash() {
    Color_t colorToMove = InterpretFEN(someFen, &info, &stack);
    ZobristHash_t hash = HashPosition(&info, &stack, colorToMove);
    PrintResults(hash);
}

void ZobristTDDRunner() {
    ShouldGenerateNonZeroHash();
}