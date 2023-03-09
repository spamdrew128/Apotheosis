#include "basic_tests.h"
#include "debug.h"
#include "fen.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"
#include "UCI.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

enum {
    large_time = 100000
};

static void ShouldFindM2() {
    FEN_t fen = "r7/4n2p/1p4p1/6P1/2k2P2/1q6/7K/8 b - - 25 68";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);
    
    UciSearchInfo_t uciSearchInfo;
    UciSearchInfoInit(&uciSearchInfo);

    uciSearchInfo.forceTime = large_time;
    uciSearchInfo.depthLimit = 6;

    SearchResults_t results = Search(&uciSearchInfo, &boardInfo, &gameStack, &zobristStack, false);

    Move_t expectedBestMove;
    UCITranslateMove(&expectedBestMove, "a8a2", &boardInfo, &gameStack);

    PrintResults(CompareMoves(results.bestMove, expectedBestMove));

    TeardownTT(&uciSearchInfo.tt);
}

void BasicTestsRunner() {
    ShouldFindM2();
}