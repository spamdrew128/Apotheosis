#ifndef __MAKE_AND_UNMAKE_H__
#define __MAKE_AND_UNMAKE_H__

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "movegen.h"

void MakeMove(BoardInfo_t* boardInfo, Move_t move, Color_t color);

#endif