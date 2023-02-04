#include <assert.h>

#include "recursive_testing.h"
#include "debug.h"
#include "game_state_old.h"
#include "movegen.h"
#include "make_and_unmake.h"

static bool UnmakeSuccess(BoardInfo_t* info, BoardInfo_t* originalInfo, GameStateOld_t* initalState) {
    bool infoMatches = CompareInfo(info, originalInfo);
    bool stateMatches = CompareStateOld(initalState);

    if(!(infoMatches && stateMatches)) {
        printf("\ninfoMatches: %d\n", infoMatches);
        printf("stateMatches: %d\n", stateMatches);
        return false;
    }

    return true;
}

static int tests;

static void UnmakeTest(BoardInfo_t* boardInfo, int depth, Color_t color) {
    if(depth == 0) {
        return;
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, color);
    BoardInfo_t initialInfo = *boardInfo;
    GameStateOld_t initalState = ReadCurrentGameStateOld();

    for(int i = 0; i <= moveList.maxIndex; i++) {
        tests++;

        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, move, color);

        UnmakeTest(boardInfo, depth-1, !color);

        UnmakeMove(boardInfo, move, color);

        if(!UnmakeSuccess(boardInfo, &initialInfo, &initalState)) {
            PrintChessboard(&initialInfo);
            PrintMove(move);
            PrintChessboard(boardInfo);
            printf("test number %d\n", tests);
            assert(false);
        }
    }
}

void UnmakeRecursiveTestRunner(FEN_t fen, int depth, bool runTests) {
    tests = 0;
    if(runTests) {
        BoardInfo_t info;
        Color_t color = InterpretFEN(fen, &info);
        UnmakeTest(&info, depth, color);
    }
}

static void SplitPERFT(BoardInfo_t* boardInfo, int depth, Color_t color) {
    if(depth == 0) {
        return;
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, color);

    for(int i = 0; i <= moveList.maxIndex; i++) {
        tests++;
        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, move, color);

        SplitPERFT(boardInfo, depth-1, !color);

        UnmakeMove(boardInfo, move, color);
    }
}

void PERFTRunner(FEN_t fen, int depth, bool runTests) {
    tests = 0;
    if(runTests) {
        BoardInfo_t info;
        Color_t color = InterpretFEN(fen, &info);
        SplitPERFT(&info, depth, color);
        printf("%d test run\n", tests);
    }
}