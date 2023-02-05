#include <assert.h>
#include <time.h>

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

        UnmakeMove(boardInfo, &stack);

        if(!UnmakeSuccess(boardInfo, &initialInfo, &initalState)) {
            PrintChessboard(&initialInfo);
            PrintMove(move, true);
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

static void _SplitPERFTHelper(BoardInfo_t* boardInfo, int depth, PerftCount_t* count, Color_t color) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &stack, color);

    if(depth > 1) {
        for(int i = 0; i <= moveList.maxIndex; i++) {
            Move_t move = moveList.moves[i];
            MakeMove(boardInfo, &stack, move, color);

            _SplitPERFTHelper(boardInfo, depth-1, count, !color);

            UnmakeMove(boardInfo, &stack);
        }
    } else {
        *count += moveList.maxIndex + 1;
        return;
    }
}

static void PrintSplitPerftResults(Move_t move, PerftCount_t count) {
    PrintMove(move, false);
    printf(" - %lld\n", count);
}

static PerftCount_t SplitPERFT(BoardInfo_t* boardInfo, int depth, Color_t color) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &stack, color);
    PerftCount_t total = 0;

    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, &stack, move, color);

        PerftCount_t count = 0;
        if(depth > 1) {
            _SplitPERFTHelper(boardInfo, depth-1, &count, !color);
            PrintSplitPerftResults(move, count);
            total += count;
        } else {
            PrintSplitPerftResults(move, 1);
            total += 1;
        }

        UnmakeMove(boardInfo, &stack);
    }

    return total;
}

void PERFTRunner(FEN_t fen, int depth, bool runTests) {
    TestSetup();
    if(runTests) {
        BoardInfo_t info;
        Color_t color = InterpretFEN(fen, &info, &stack);

        printf("\n");
        PerftCount_t totalCount = SplitPERFT(&info, depth, color);
        printf("\nDepth %d found %lld positions\n", depth, totalCount);
    }
}

static void FullySearchTree(BoardInfo_t* boardInfo, int depth, PerftCount_t* count, Color_t color) {
    if(depth == 0) {
        (*count)++;
        return;
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &stack, color);
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i];
        MakeMove(boardInfo, &stack, move, color);

        FullySearchTree(boardInfo, depth-1, count, !color);

        UnmakeMove(boardInfo, &stack);
    }
}

void SpeedTest(FEN_t fen, int depth, bool runTests) {
    TestSetup();
    if(runTests) {
        BoardInfo_t info;
        Color_t color = InterpretFEN(fen, &info, &stack);

        clock_t t;
        t = clock();

        printf("\n");
        PerftCount_t totalCount = 0;
        FullySearchTree(&info, depth, &totalCount, color);

        t = clock() - t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC;
        printf("%f\n", time_taken);
    }
}