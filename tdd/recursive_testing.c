#include <assert.h>

#include "recursive_testing.h"
#include "debug.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"

typedef uint64_t PerftCount_t;

static GameStack_t stack;

static void TestSetup() {
    InitGameStack(&stack);
}

static bool UnmakeSuccess(BoardInfo_t* info, BoardInfo_t* originalInfo, GameState_t* initalState) {
    bool infoMatches = CompareInfo(info, originalInfo);
    bool stateMatches = CompareState(initalState, &stack);

    if(!(infoMatches && stateMatches)) {
        printf("\ninfoMatches: %d\n", infoMatches);
        printf("stateMatches: %d\n", stateMatches);
        return false;
    }

    return true;
}

static PerftCount_t tests;
static void UnmakeTest(BoardInfo_t* boardInfo, int depth, Color_t color) {
    if(depth == 0) {
        return;
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &stack, color);
    BoardInfo_t initialInfo = *boardInfo;
    GameState_t initalState = ReadCurrentGameState(&stack);

    for(int i = 0; i <= moveList.maxIndex; i++) {
        tests++;

        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, &stack, move, color);

        UnmakeTest(boardInfo, depth-1, !color);

        UnmakeMove(boardInfo, &stack, move, color);

        if(!UnmakeSuccess(boardInfo, &initialInfo, &initalState)) {
            PrintChessboard(&initialInfo);
            PrintMove(move);
            PrintChessboard(boardInfo);
            printf("test number %lld\n", tests);
            assert(false);
        }
    }
}

void UnmakeRecursiveTestRunner(FEN_t fen, int depth, bool runTests) {
    tests = 0;
    TestSetup();
    if(runTests) {
        BoardInfo_t info;
        Color_t color = InterpretFEN(fen, &info, &stack);
        UnmakeTest(&info, depth, color);
    }
}

static void SplitPERFT(BoardInfo_t* boardInfo, int depth, PerftCount_t* count, Color_t color) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &stack, color);

    if(depth > 1) {
        for(int i = 0; i <= moveList.maxIndex; i++) {
            Move_t move = moveList.moves[i];
            MakeMove(boardInfo, &stack, move, color);

            SplitPERFT(boardInfo, depth-1, count, !color);

            UnmakeMove(boardInfo, &stack, move, color);
        }
    } else {
        *count += moveList.maxIndex + 1;
        return;
    }
}

void PERFTRunner(FEN_t fen, int depth, bool runTests) {
    TestSetup();
    if(runTests) {
        BoardInfo_t info;
        Color_t color = InterpretFEN(fen, &info, &stack);

        PerftCount_t count = 0;
        SplitPERFT(&info, depth, &count, color);
        printf("\nDepth %d found %lld positions\n", depth, count);
    }
}