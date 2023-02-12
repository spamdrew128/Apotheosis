#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"
#include "lookup.h"
#include "FEN.h"
#include "pieces.h"
#include "magic.h"
#include "game_state.h"
#include "zobrist.h"

int main(int argc, char** argv)
{
    InitLookupTables();
    GenerateZobristKeys();

    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    InterpretFEN(START_FEN, &boardInfo, &gameStack, &zobristStack);

    
    // Todo: GAME LOOP
    
}