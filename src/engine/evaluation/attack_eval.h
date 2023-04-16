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

enum {
  king_attack = 0,
  pawn_attack = 0,
  minor_attack = 2,
  rook_attack = 3,
  queen_attack = 5,

  inner_ring_bonus = 1,

  queen_piece_weight = 2,
  other_piece_weight = 1,
};

typedef int AttackScore_t;
typedef struct {
  int attackerCount;
  AttackScore_t attackScore;
  Bitboard_t attackZone;
  Bitboard_t innerKingRing;
} AttackInfo_t;

void MobilitySafetyThreatsEval(BoardInfo_t* boardInfo, Score_t* score);

void TDDMobilityStuff(BoardInfo_t* boardInfo, AttackInfo_t* wAttack, AttackInfo_t* bAttack, Score_t* nonSafetyScore);

#endif