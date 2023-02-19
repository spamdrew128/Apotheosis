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

static PlayerTimeInfo_t GetTimeInfo() {
    PlayerTimeInfo_t uciTimeInfo;
    uciTimeInfo.wTime = large_time;
    uciTimeInfo.bTime = large_time;
    uciTimeInfo.wInc = 0;
    uciTimeInfo.bInc = 0;

    return uciTimeInfo;
}

static void ShouldFindM2() {
    FEN_t fen = "r7/4n2p/1p4p1/6P1/2k2P2/1q6/7K/8 b - - 25 68";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);
    
    PlayerTimeInfo_t uciTimeInfo = GetTimeInfo();
    SearchResults_t results = Search(uciTimeInfo, &boardInfo, &gameStack, &zobristStack, 6);

    Move_t expectedBestMove;
    UCITranslateMove(&expectedBestMove, "a8a2", &boardInfo, &gameStack);

    PrintResults(CompareMoves(results.bestMove, expectedBestMove));
}

void BasicTestsRunner() {
    ShouldFindM2();
}