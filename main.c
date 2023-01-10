#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"
#include "lookup.h"
#include "FEN.h"
#include "pieces.h"
#include "magic.h"

int main(int argc, char *argv[])
{
    SeedRNG();
    
    InitLookup();
    BoardInfo_t boardInfo;
    InitBoardInfo(&boardInfo);
    InterpretFEN(START_FEN, &boardInfo);
    
}