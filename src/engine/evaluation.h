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

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo);

#endif