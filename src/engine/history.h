#ifndef __HISTORY_H__
#define __HISTORY_H__

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "engine_types.h"

typedef struct {
    Move_t moves[NUM_PIECES][NUM_SQUARES];
} History_t;

#endif