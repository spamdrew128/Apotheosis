#include <assert.h>

#include "recursive_testing.h"
#include "debug.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "zobrist.h"
#include "timer.h"

typedef unsigned long long PerftCount_t;

static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void TestSetup() {
    InitGameStack(&gameStack);
}

static bool UnmakeSuccess(BoardInfo_t* info, BoardInfo_t* originalInfo, GameState_t* initalState) {
    bool infoMatches = CompareInfo(info, originalInfo);
    bool stateMatches = CompareState(initalState, &gameStack);

    if(!(infoMatches && stateMatches)) {
        printf("\ninfoMatches: %d\n", infoMatches);
        printf("stateMatches: %d\n", stateMatches);
        return false;
    }

    return true;
}

static PerftCount_t tests;
static void UnmakeTest(BoardInfo_t* boardInfo, int depth) {
    if(depth == 0) {
        return;
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &gameStack);
    BoardInfo_t initialInfo = *boardInfo;
    GameState_t initalState = ReadCurrentGameState(&gameStack);

    for(int i = 0; i <= moveList.maxIndex; i++) {
        tests++;

        Move_t move = moveList.moves[i].move;
        MakeMove(boardInfo, &gameStack, move);

        UnmakeTest(boardInfo, depth-1);

        UnmakeMove(boardInfo, &gameStack);

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
        InterpretFEN(fen, &info, &gameStack, &zobristStack);
        UnmakeTest(&info, depth);
    }
}

static void _SplitPERFTHelper(BoardInfo_t* boardInfo, int depth, PerftCount_t* count) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &gameStack);

    if(depth > 1) {
        for(int i = 0; i <= moveList.maxIndex; i++) {
            Move_t move = moveList.moves[i].move;
            MakeMove(boardInfo, &gameStack, move);

            _SplitPERFTHelper(boardInfo, depth-1, count);

            UnmakeMove(boardInfo, &gameStack);
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

static PerftCount_t SplitPERFT(BoardInfo_t* boardInfo, int depth) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &gameStack);
    PerftCount_t total = 0;

    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i].move;
        MakeMove(boardInfo, &gameStack, move);
        assert(BoardIsValid(boardInfo, &gameStack));

        PerftCount_t count = 0;
        if(depth > 1) {
            _SplitPERFTHelper(boardInfo, depth-1, &count);
            PrintSplitPerftResults(move, count);
            total += count;
        } else {
            PrintSplitPerftResults(move, 1);
            total += 1;
        }

        UnmakeMove(boardInfo, &gameStack);
        assert(BoardIsValid(boardInfo, &gameStack));
    }

    return total;
}

void PERFTRunner(FEN_t fen, int depth, bool runTests) {
    TestSetup();
    if(runTests) {
        BoardInfo_t info;
        InterpretFEN(fen, &info, &gameStack, &zobristStack);

        printf("\n");
        PerftCount_t totalCount = SplitPERFT(&info, depth);
        printf("\nDepth %d found %lld positions\n", depth, totalCount);
    }
}

static void FullySearchTree(BoardInfo_t* boardInfo, int depth, PerftCount_t* count) {
    if(depth == 0) {
        (*count)++;
        return;
    }

    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &gameStack);
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Move_t move = moveList.moves[i].move;
        MakeMove(boardInfo, &gameStack, move);

        FullySearchTree(boardInfo, depth-1, count);

        UnmakeMove(boardInfo, &gameStack);
    }
}

void SpeedTest(FEN_t fen, int depth, bool runTests) {
    TestSetup();
    if(runTests) {
        BoardInfo_t info;
        InterpretFEN(fen, &info, &gameStack, &zobristStack);

        Stopwatch_t stopwatch;
        StopwatchInit(&stopwatch);

        printf("\n");
        PerftCount_t totalCount = 0;
        FullySearchTree(&info, depth, &totalCount);

        double time_taken = ElapsedTime(&stopwatch);
        double nodes = (double)totalCount;
        double MNPS = (nodes / 1000000) / time_taken*msec_per_sec;

        printf("%lld Nodes in %f seconds\n", totalCount, time_taken/msec_per_sec);
        printf("%f MNPS\n", MNPS);
    }
}