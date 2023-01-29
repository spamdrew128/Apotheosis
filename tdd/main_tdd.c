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
    MovegenTDDRunner();
    GameStateTDDRunner();

    TeardownLookup();
}