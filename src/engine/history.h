#ifndef __HISTORY_H__
#define __HISTORY_H__

#include "board_constants.h"
#include "bitboards.h"
#include "movegen.h"
#include "engine_types.h"

typedef struct {
    MoveScore_t scores[NUM_PIECES][NUM_SQUARES];
} History_t;

void InitHistory(History_t* history);

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, Move_t move, Depth_t depth);

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move);

#endif