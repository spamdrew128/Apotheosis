#ifndef __MOVE_ORDERING_H__
#define __MOVE_ORDERING_H__

#include "movegen.h"
#include "board_info.h"
#include "board_constants.h"

void SortCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo);

void SortTTMove(MoveList_t* moveList, Move_t ttMove, int maxIndex);

#endif