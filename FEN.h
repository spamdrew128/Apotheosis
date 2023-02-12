#ifndef __FEN_H__
#define __FEN_H__

#include "bitboards.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"

typedef const char* FEN_t;

Color_t InterpretFEN(
    FEN_t fen,
    BoardInfo_t* info,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
);

#endif