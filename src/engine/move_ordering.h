#ifndef __MOVE_ORDERING_H__
#define __MOVE_ORDERING_H__

#include "movegen.h"
#include "board_info.h"
#include "board_constants.h"
#include "killers.h"
#include "history.h"

typedef struct {
    MoveEntryList_t* moveList;
    MoveIndex_t headIndex;
    MoveIndex_t tailIndex;
} MovePicker_t;

void InitAllMovePicker(
    MovePicker_t* movePicker,
    MoveEntryList_t* moveList,
    BoardInfo_t* boardInfo,
    Move_t ttMove,
    Killers_t* killers,
    History_t* history,
    Ply_t ply
);

void InitCaptureMovePicker(
    MovePicker_t* movePicker,
    MoveEntryList_t* moveList,
    BoardInfo_t* boardInfo
);

Move_t PickMove(MovePicker_t* movePicker);

#endif