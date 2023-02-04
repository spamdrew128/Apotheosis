#include "recursive_testing.h"
#include "debug.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"

static bool UnmakeSuccess(BoardInfo_t* info, BoardInfo_t* originalInfo, GameState_t* initalState) {
    bool infoMatches = CompareInfo(info, originalInfo);
    bool stateMatches = CompareState(initalState);

    return infoMatches && stateMatches;
}

static void UnmakeTest(BoardInfo_t* boardInfo, int depth, Color_t color) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, color);
    BoardInfo_t initialInfo = *boardInfo;
    GameState_t initalState = ReadCurrentGameState();

    bool success = UnmakeSuccess(boardInfo, &initialInfo, &initalState);
    if(!success) {
        printf("FAIL");
    }
}

void UnmakeRecursiveTestRunner(FEN_t fen, int depth, bool runTests) {
    BoardInfo_t info;
    Color_t color = InterpretFEN(fen, &info);
    UnmakeTest(&info, depth, color);
}

void PERFTRunner(FEN_t fen, int depth, bool runTests) {

}