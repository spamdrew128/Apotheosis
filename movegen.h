#ifndef __MOVEGEN_H__
#define __MOVEGEN_H__

#include "board_constants.h"
#include "move.h"

typedef struct {
    Move_t moves[MOVELIST_MAX];
    int maxIndex;
} MoveList_t;

void CapturesMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color);

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color);

#endif