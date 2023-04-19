#ifndef ATTACK_EVAL_H
#define ATTACK_EVAL_H

#include <stdio.h>
#include "board_info.h"
#include "eval_constants.h"
#include "eval_constants_tools.h"

typedef uint8_t VirtualMobility_t;
enum {
  KNIGHT_MOBILITY_OPTIONS = 9,
  BISHOP_MOBILITY_OPTIONS = 14,
  ROOK_MOBILITY_OPTIONS = 15,
  QUEEN_MOBILITY_OPTIONS = BISHOP_MOBILITY_OPTIONS + ROOK_MOBILITY_OPTIONS - 1,

  KING_VIRTUAL_MOBILITY_OPTIONS = QUEEN_MOBILITY_OPTIONS,
};

VirtualMobility_t KingVirtualMobility(BoardInfo_t* boardInfo, Bitboard_t color);

void MobilityAndThreatsEval(BoardInfo_t* boardInfo, Score_t* score);

#endif