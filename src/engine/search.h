#ifndef __SEARCH_H__
#define __SEARCH_H__

#include <stdbool.h>
#include <stdint.h>

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"
#include "zobrist.h"
#include "evaluation.h"

typedef uint16_t Milliseconds_t;
typedef struct
{
    Milliseconds_t wTime;
    Milliseconds_t bTime;
    Milliseconds_t wInc;
    Milliseconds_t bInc;
} PlayerTimeInfo_t;

typedef struct {
    Move_t bestMove;
    EvalScore_t score;
} SearchResults_t;

typedef uint8_t Depth_t;

SearchResults_t Search(
    PlayerTimeInfo_t uciTimeInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    Depth_t maxDepth
);

#endif