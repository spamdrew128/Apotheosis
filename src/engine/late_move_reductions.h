#ifndef LATE_MOVE_REDUCTIONS_H
#define LATE_MOVE_REDUCTIONS_H

#include <stdint.h>
#include "engine_types.h"

typedef uint32_t Reduction_t;
typedef uint16_t MoveCount_t;

void InitReductionTable();

Reduction_t GetReduction(Depth_t depth, MoveCount_t moveCount);

#endif