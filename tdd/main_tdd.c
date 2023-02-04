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

    UnmakeRecursiveTestRunner("r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R w Kq - 0 1", 5, false);
    PERFTRunner(START_FEN, 2, true);

    TeardownLookup();
}