#include "basic_tests.h"
#include "debug.h"
#include "fen.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void ShouldFindM2() {
    FEN_t fen = "r7/4n2p/1p4p1/6P1/2k2P2/1q6/7K/8 b - - 25 68";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);
    
    PlayerTimeInfo_t uciTimeInfo;
    SearchResults_t results = Search(uciTimeInfo, &boardInfo, &gameStack, &zobristStack);
}

void BasicTestsRunner() {
    ShouldFindM2();
}