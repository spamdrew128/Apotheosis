#ifndef __MOVEGEN_H__
#define __MOVEGEN_H__

#include <stdint.h>
#include <limits.h>

#include "board_constants.h"
#include "board_info.h"
#include "move.h"
#include "game_state.h"

enum {
    movelist_empty = -1,

    ENTRY_MAX_SCORE = INT32_MAX,
    ENTRY_MIN_SCORE = INT32_MIN,
};

typedef int32_t MoveScore_t;
typedef int32_t MoveIndex_t;

enum MoveScores { // for move ordering later
    tt_score = ENTRY_MAX_SCORE,
    promotion_score = tt_score - 1,
    capture_offset = promotion_score - 2000,
    killer_base_score = capture_offset - 2000,
    quiet_score = ENTRY_MIN_SCORE,
};

typedef struct {
    Move_t move;
    MoveScore_t score; 
} MoveEntry_t;

typedef struct {
    MoveEntry_t moves[MOVELIST_MAX];
    MoveIndex_t maxCapturesIndex;
    MoveIndex_t maxIndex;
} MoveList_t;

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack);

bool EnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t toBB, Bitboard_t fromBB, Color_t color);

#endif
