#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"
#include "lookup.h"
#include "FEN.h"
#include "pieces.h"
#include "magic.h"
#include "game_state.h"
#include "zobrist.h"
#include "UCI.h"

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    InitLookupTables();
    GenerateZobristKeys();

    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;

    bool running = true;
    while(running)
    {
        running = InterpretUCIInput(&boardInfo, &gameStack, &zobristStack);
    }
}