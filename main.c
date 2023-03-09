#include <stdio.h>
#include <string.h>

#include "board_constants.h"
#include "bitboards.h"
#include "lookup.h"
#include "FEN.h"
#include "pieces.h"
#include "magic.h"
#include "game_state.h"
#include "zobrist.h"
#include "UCI.h"
#include "bench.h"
#include "chess_search.h"
#include "transposition_table.h"

static void ProgramTeardown(UciApplicationData_t* uciApplicationData) {
    TranspositionTable_t* tt = &uciApplicationData->uciSearchInfo.tt;
    TeardownTT(tt);
}

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    InitLookupTables();
    GenerateZobristKeys();

    bool running = Bench(argc, argv);

    UciApplicationData_t uciApplicationData;
    UciSearchInfoInit(&uciApplicationData.uciSearchInfo);
    while(running) {
        running = InterpretUCIInput(&uciApplicationData);
    }

    ProgramTeardown(&uciApplicationData);
}