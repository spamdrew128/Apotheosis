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
#include "engine_types.h"
#include "transposition_table.h"

enum UciSearchInfoOptions {
    overhead_default_msec = 50,
    overhead_min_msec = 1,
    overhead_max_msec = 128,

    hash_default_mb = 16,
    hash_min_mb = 1,
    hash_max_mb = 4096,
};

typedef struct {
    Milliseconds_t wTime;
    Milliseconds_t bTime;
    Milliseconds_t wInc;
    Milliseconds_t bInc;
    Milliseconds_t forceTime;
    Milliseconds_t overhead;

    Depth_t depthLimit;

    TranspositionTable_t tt;
} UciSearchInfo_t;

typedef struct {
    Move_t bestMove;
    EvalScore_t score;
} SearchResults_t;

SearchResults_t Search(
    UciSearchInfo_t* uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    bool printUciInfo
);

NodeCount_t BenchSearch(
    UciSearchInfo_t* uciSearchInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
);

void UciSearchInfoTimeInfoReset(UciSearchInfo_t* uciSearchInfo);

void UciSearchInfoInit(UciSearchInfo_t* uciSearchInfo);

#endif