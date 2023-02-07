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

int main(int argc, char** argv)
{
    InitLookup();

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

    SpeedTest(START_FEN, 5, false);

    FEN_t fen = "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1";
    PERFTRunner(fen, 1, true);
    RunAllPerftTests(false);

    TeardownLookup();
}