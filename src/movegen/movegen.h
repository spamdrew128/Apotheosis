#ifndef __MOVEGEN_H__
#define __MOVEGEN_H__

#include "board_constants.h"
#include "board_info.h"
#include "move.h"
#include "game_state.h"

enum {
    movelist_empty = -1
};

typedef struct {
    Move_t moves[MOVELIST_MAX];
    int maxIndex;
} MoveList_t;

void QSearchMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack);

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack);

#endif
