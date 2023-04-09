#ifndef BOARD_CONTROL_H
#define BOARD_CONTROL_H

#include "engine_types.h"
#include "eval_constants.h"
#include "eval_constants_tools.h"
#include "bitboards.h"

typedef int ControlValue_t;
enum {
    knight_control = 700,
    bishop_control = 700,
    rook_control = 300,
    queen_control = 100,
    pawn_control = 9000,
};

void BoardControl(
    BoardInfo_t* boardInfo,
    const Bucket_t wBucket,
    const Bucket_t bBucket,
    Score_t* score
);

#endif
