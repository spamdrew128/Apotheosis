#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"
#include "lookup.h"
#include "FEN.h"
#include "pieces.h"
#include "magic.h"
#include "game_state.h"

int main(int argc, char** argv)
{
    InitLookup();
    BoardInfo_t boardInfo;
    InitBoardInfo(&boardInfo);
    GameStack_t stack;
    InterpretFEN(START_FEN, &boardInfo, &stack);
    
    // Todo: GAME LOOP
    
}