#ifndef __MAKE_AND_UNMAKE_H__
#define __MAKE_AND_UNMAKE_H__

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "movegen.h"
#include "game_state.h"
#include "zobrist.h"

void MakeMove(BoardInfo_t* boardInfo, GameStack_t* gameStack, Move_t move);

void UnmakeMove(BoardInfo_t* boardInfo, GameStack_t* gameStack);

void MakeNullMove(BoardInfo_t* boardInfo, GameStack_t* gameStack, ZobristStack_t* zobristStack);

#endif