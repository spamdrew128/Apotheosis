#ifndef __MOVE_ORDERING_H__
#define __MOVE_ORDERING_H__

#include "movegen.h"
#include "board_info.h"
#include "board_constants.h"

typedef struct {
    MoveList_t* moveList;
    MoveIndex_t headIndex;
    MoveIndex_t tailIndex;
} MovePicker_t;

void InitMovePicker(
    MovePicker_t* movePicker,
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Move_t ttMove,
    MoveIndex_t finalIndex
);

Move_t PickMove(MovePicker_t* movePicker);

#endif