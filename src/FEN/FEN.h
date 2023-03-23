#ifndef __FEN_H__
#define __FEN_H__

#include "bitboards.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"
#include "board_constants.h"

typedef const char* FEN_t;

void InterpretFEN(
    FEN_t fen,
    BoardInfo_t* info,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
);

void BoardToFEN(
    BoardInfo_t* info,
    GameStack_t* gameStack,
    char result[2000]
);

void PrintFEN(BoardInfo_t* info, GameStack_t* gameStack);

#endif