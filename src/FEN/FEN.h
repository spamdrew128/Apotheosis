#ifndef __FEN_H__
#define __FEN_H__

#include "bitboards.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"
#include "board_constants.h"

typedef const char* FEN_t;
#define FEN_BUFFER_SIZE 100

void InterpretFEN(
    FEN_t fen,
    BoardInfo_t* info,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
);

void BoardToFEN(
    BoardInfo_t* info,
    GameStack_t* gameStack,
    char result[FEN_BUFFER_SIZE]
);

void PrintFEN(BoardInfo_t* info, GameStack_t* gameStack);

bool FENsMatch(FEN_t expected, FEN_t actual);

#endif