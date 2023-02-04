#include "recursive_testing.h"
#include "debug.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"

static void UnmakeTest(BoardInfo_t* boardInfo, int depth, Color_t color) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, color);
    GameState_t currentState = ReadCurrentGameState();
}

void UnmakeRecursiveTestRunner(bool runTests, int depth, FEN_t fen) {
    BoardInfo_t info;
    Color_t color = InterpretFEN(fen, &info);
    UnmakeTest(&info, depth, color);
}

void PERFTRunner(bool runTests, int depth, FEN_t fen) {

}