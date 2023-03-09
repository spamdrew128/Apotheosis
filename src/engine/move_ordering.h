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

#endif