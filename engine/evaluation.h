#ifndef __EVALUATION_H__
#define __EVALUATION_H__

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"

typedef int16_t EvalScore_t;
enum {
    EVAL_MAX = 10000
};

typedef int16_t Centipawns_t;
enum {
  knight_value = 300,
  bishop_value = 320,
  rook_value = 500,
  queen_value = 900,
  pawn_value = 100,
  king_value = 400
};

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo);

#endif