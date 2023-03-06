#include <stdio.h>
#include <string.h>

#include "bench.h"
#include "timer.h"
#include "perft_table_entries.h"
#include "chess_search.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"
#include "FEN.h"

bool Bench(int argc, char** argv) {
    if(argc != 2 || strcmp(argv[1], "bench")) {
        return true; // keep running
    }

    FEN_t fenList[] = { PERFT_TEST_TABLE(EXPAND_AS_FEN_ARRAY) };
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;

    Stopwatch_t stopwatch;
    StopwatchInit(&stopwatch);

    UciSearchInfo_t uciSearchInfo;
    UciSearchInfoInit(&uciSearchInfo);
    uciSearchInfo.forceTime = 1000000;
    uciSearchInfo.depthLimit = 5;

    NodeCount_t nodeCount = 0;
    for(int i = 0; i < NUM_PERFT_ENTRIES; i++) {
        InterpretFEN(fenList[i], &boardInfo, &gameStack, &zobristStack);
        gameStack.gameStates[gameStack.top].halfmoveClock = 0;
        nodeCount += BenchSearch(&uciSearchInfo, &boardInfo, &gameStack, &zobristStack);
    }

    Milliseconds_t msec = ElapsedTime(&stopwatch);
    printf("%lld nodes %lld nps\n", (long long)nodeCount, (long long)(nodeCount * msec_per_sec) / msec);

    TeardownTT(&uciSearchInfo.tt);

    return false;
}