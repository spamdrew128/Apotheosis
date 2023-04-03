#ifndef __PAWN_STRUCTURE_H__
#define __PAWN_STRUCTURE_H__

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "board_info.h"
#include "engine_types.h"
#include "eval_constants.h"

void PassedPawns(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore);

#endif