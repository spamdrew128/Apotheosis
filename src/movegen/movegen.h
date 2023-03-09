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
    
    ENTRY_MAX_SCORE = INT16_MAX,
    ENTRY_MIN_SCORE = INT16_MIN,
};

typedef int16_t MoveScore_t;
typedef uint8_t MoveIndex_t;

typedef struct {
    Move_t move;
    MoveScore_t score; // for move ordering later
} MoveEntry_t;

typedef struct {
    MoveEntry_t moves[MOVELIST_MAX];
    MoveIndex_t maxCapturesIndex;
    MoveIndex_t maxIndex;
} MoveList_t;

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack);

bool EnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t toBB, Bitboard_t fromBB, Color_t color);

#endif
