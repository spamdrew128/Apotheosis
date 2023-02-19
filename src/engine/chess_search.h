#ifndef __CHESS_SEARCH_H__
#define __CHESS_SEARCH_H__

#include <stdbool.h>
#include <limits.h>

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"
#include "zobrist.h"
#include "evaluation.h"
#include "time_constants.h"

enum {
    MS_MAX_TIME = UINT32_MAX
};

typedef uint8_t Depth_t;
typedef uint8_t Ply_t;

typedef struct
{
    Milliseconds_t wTime;
    Milliseconds_t bTime;
    Milliseconds_t wInc;
    Milliseconds_t bInc;
    Milliseconds_t timeLimit;

    Depth_t depthLimit;
} UciSearchInfo_t;

typedef struct {
    Move_t bestMove;
    EvalScore_t score;
} SearchResults_t;

SearchResults_t Search(
    UciSearchInfo_t uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
);

void UciSearchInfoInit(UciSearchInfo_t* uciSearchInfo);

#endif