#include "recursive_testing.h"
#include "debug.h"
#include "FEN.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"

static void UnmakeTest(BoardInfo_t* boardInfo, int depth) {
    
}

void UnmakeRecursiveTestRunner(bool runTests, int depth, FEN_t fen) {
    BoardInfo_t* info;
    InterpretFEN(fen, &info);
}

void PERFTRunner(bool runTests, int depth, FEN_t fen) {

}