#ifndef __PAWN_STRUCTURE_H__
#define __PAWN_STRUCTURE_H__

#include <stdbool.h>
#include <stdint.h>

#include "board_constants.h"
#include "bitboards.h"
#include "board_info.h"
#include "engine_types.h"
#include "eval_constants.h"

void PassedPawnBonus(
    BoardInfo_t* boardInfo,
    Score_t* score
);

void OpenFileBonus(
    BoardInfo_t* boardInfo,
    Score_t* score
);

#endif