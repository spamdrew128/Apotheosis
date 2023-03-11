#ifndef __KILLERS_H__
#define __KILLERS_H__

#include "board_constants.h"
#include "bitboards.h"
#include "move.h"
#include "engine_types.h"

enum {
    KILLERS_LENGTH = PLY_MAX,
    KILLERS_WIDTH = 2,
};

typedef struct {
    Move_t moves[KILLERS_LENGTH][KILLERS_WIDTH];
} Killers_t;

void InitKillers(Killers_t* killers);

void AddKiller(Killers_t* killers, Move_t move, Ply_t ply);

Move_t GetKiller(Killers_t* killers, Ply_t ply, int index);

#endif