#ifndef __MOVE_ORDERING_H__
#define __MOVE_ORDERING_H__

#include <stdint.h>

#include "movegen.h"
#include "board_info.h"
#include "board_constants.h"
#include "engine_types.h"

typedef int32_t IteratorScore_t;
typedef struct {
    Move_t move;
    IteratorScore_t score;
} MoveListIteratorItem_t;

typedef struct {
    MoveListIteratorItem_t itemList[MOVELIST_MAX];
    int headIndex;
    int maxIndex;
} MoveListIterator_t;

void MoveListIteratorInit(MoveListIterator_t* iterator, MoveList_t* moveList, BoardInfo_t* boardInfo, int maxIndex);

Move_t IteratorGetNextMove(MoveListIterator_t* iterator);

void SortMoveList(MoveList_t* moveList, BoardInfo_t* boardInfo);

#endif