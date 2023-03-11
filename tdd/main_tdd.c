#include <stdio.h>

#include "lookup.h"
#include "board_constants.h"
#include "lookup_tdd.h"
#include "bitboards_tdd.h"
#include "FEN_tdd.h"
#include "pieces_tdd.h"
#include "magic_tdd.h"
#include "legals_tdd.h"
#include "movegen_tdd.h"
#include "game_state_tdd.h"
#include "board_info_tdd.h"
#include "make_and_unmake_tdd.h"
#include "recursive_testing.h"
#include "perft_table.h"
#include "zobrist_tdd.h"
#include "endings_tdd.h"
#include "UCI.h"
#include "basic_tests.h"
#include "PV_table_tdd.h"
#include "random_crashes.h"
#include "move_ordering_tdd.h"
#include "TT_tdd.h"
#include "killers_tdd.h"
#include "history_tdd.h"

static void ProgramTeardown(UciApplicationData_t* uciApplicationData) {
    TranspositionTable_t* tt = &uciApplicationData->uciSearchInfo.tt;
    TeardownTT(tt);
}

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    InitLookupTables();
    GenerateZobristKeys();

    LookupTDDRunner();
    BitboardsTDDRunner();
    BoardInfoTDDRunner();
    FENTDDRunner();
    PiecesTDDRunner();
    MagicTDDRunner();
    LegalsTDDRunner();
    GameStateTDDRunner();
    MovegenTDDRunner();
    MakeMoveTDDRunner();
    UnmakeMoveTDDRunner();
    ZobristTDDRunner();
    EndingsTDDRunner();

    // ENGINE TESTS
    BasicTestsRunner();
    PvTableTDDRunner();
    MoveOrderingTDDRunner();
    TranspositionTableTDDRunner();
    KillersTDDRunner();
    HistoryTDDRunner();

    // RANDOM CRASHES
    RandomCrashTestRunner(false);

    printf("\n");

    SpeedTest(START_FEN, 6, false);

    FEN_t fen = "4k3/8/8/1rpP2K1/8/8/8/8 w - c6 0 1";
    PERFTRunner(fen, 1, false);
    RunAllPerftTests(false);
    
    UciApplicationData_t uciApplicationData;
    UciSearchInfoInit(&uciApplicationData.uciSearchInfo);
    bool running = false;
    while(running)
    {
        running = InterpretUCIInput(&uciApplicationData);
    }

    ProgramTeardown(&uciApplicationData);
}