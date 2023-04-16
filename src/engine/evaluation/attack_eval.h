#ifndef ATTACK_EVAL_H
#define ATTACK_EVAL_H

#include "board_info.h"
#include "eval_constants.h"
#include "eval_constants_tools.h"

enum {
  KNIGHT_MOBILITY_OPTIONS = 9,
  BISHOP_MOBILITY_OPTIONS = 14,
  ROOK_MOBILITY_OPTIONS = 15,
  QUEEN_MOBILITY_OPTIONS = BISHOP_MOBILITY_OPTIONS + ROOK_MOBILITY_OPTIONS - 1,
};

void MobilitySafetyThreatsEval(BoardInfo_t* boardInfo, Score_t* score);

#endif