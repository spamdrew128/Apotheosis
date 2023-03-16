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
#include "engine_types.h"

enum {
  knight_value = 310,
  bishop_value = 330,
  rook_value = 500,
  queen_value = 900,
  pawn_value = 100,
  king_value = 0,
};

Centipawns_t ValueOfPiece(Piece_t piece);

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo);

#endif