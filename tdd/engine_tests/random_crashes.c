#include "random_crashes.h"
#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"
#include "zobrist.h"
#include "PV_table.h"
#include "UCI.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void CrashV0() {
    const char* posUciString = "position fen 4q3/5p2/2p1k3/2P1P2K/7P/8/8/8 b - - 0 1 moves f7f5";
    const char* goString = "go depth 2";
    InterpretUCIString(&boardInfo, &gameStack, &zobristStack, posUciString);
    InterpretUCIString(&boardInfo, &gameStack, &zobristStack, goString);
}

static void CrashV1() {
    const char* posUciString = "position startpos moves g1f3 a7a6 e2e4 a6a5 d2d4 a5a4 b1c3 c7c6 c1f4 a4a3 b2a3 a8a3 c3b1 a3a8 b1c3 b7b6 d1d3 b6b5 a2a3 b5b4 c3b1 b4a3 a1a3 c8a6 d3b3 d7d6 f3g5 g8h6 g5f3 e7e6 b1d2 d6d5 a3a4 h6g4 h2h3 d5e4 d2e4 g4f6 e4c5 f8c5 d4c5 f6d5 f4e5 b8d7 a4a6 d7e5 f3e5 a8a6 f1a6 d8a5 c2c3 a5a6 b3b8 e8e7 b8h8 a6a1 e1d2 a1c3 d2e2 c3e5 e2f1 e5a1 f1e2 a1h1 h8g7 d5f4 e2f3 f4h3 f3g3 h7h5 g2h3 h1g1 g3h4 g1g7 h4h5";
    const char* goString = "go depth 10";
    InterpretUCIString(&boardInfo, &gameStack, &zobristStack, posUciString);
    InterpretUCIString(&boardInfo, &gameStack, &zobristStack, goString);
}

static void CrashV2() {
    const char* posUciString = "position fen r1bqkb1r/1ppp1ppp/2n2n2/p3p3/4P3/2NB1N2/PPPP1PPP/R1BQK2R w KQkq - 0 1 moves a2a3 b7b6 b2b3 d7d6 g2g3 g7g6 h2h3 h7h6 a3a4 g6g5 g3g4 h6h5 g4h5 f6h5 h3h4 g5h4 h1h4 c6b4 d3f1 c7c6 d2d3 d8f6 c3b1 b4c2 d1c2 c6c5 b1d2 h5f4 h4h8 f6h8 d2b1 f7f6 b1a3 f6f5 e4f5 c8f5 a3b5 f5d3 f1d3 h8h1 d3f1 h1f3 c1f4 a8b8 b5c7 e8d7 c7a6 b8a8 c2f5 d7c6 f1b5 c6d5 a6c7 d5d4 f4e5 d6e5 f5f3 c5c4";
    const char* goString = "go depth 6";
    InterpretUCIString(&boardInfo, &gameStack, &zobristStack, posUciString);
    InterpretUCIString(&boardInfo, &gameStack, &zobristStack, goString);
}

void RandomCrashTestRunner(bool shouldRun) {
    if(!shouldRun) {
        return;
    }
    CrashV0();
    CrashV1();
    CrashV2();
}