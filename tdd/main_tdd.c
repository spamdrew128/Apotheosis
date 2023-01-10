#include <stdio.h>

#include "lookup.h"
#include "magic.h"
#include "board_constants.h"
#include "lookup_tdd.h"
#include "bitboards_tdd.h"
#include "FEN_tdd.h"
#include "pieces_tdd.h"
#include "magic_tdd.h"

int main(int argc, char *argv[])
{
    InitLookup();
    SeedRNG();

    LookupTDDRunner();
    BitboardsTDDRunner();
    FENTDDRunner();
    PiecesTDDRunner();
    MagicTDDRunner();
}