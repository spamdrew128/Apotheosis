#ifndef __KILLERS_H__
#define __KILLERS_H__

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "engine_types.h"

typedef struct
{
    Move_t killerMoves[2][PLY_MAX];
} Killers_t;


#endif