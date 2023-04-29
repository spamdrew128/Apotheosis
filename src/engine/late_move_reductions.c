#include <math.h>

#include "late_move_reductions.h"
#include "board_constants.h"
#include "util_macros.h"

static Reduction_t reductionTable[DEPTH_MAX][MOVELIST_MAX];

void InitReductionTable() {
    double lmrBase = 0.77;
    double lmrDivisor = 2.36;
    for(int d = 0; d < DEPTH_MAX; d++) {
        for(int m = 0; m < MOVELIST_MAX; m++) {
            double depth = MAX(d, 1);
            double move = MAX(m, 1);
            reductionTable[d][m] = 
            (uint32_t)(lmrBase + log(depth) * log(move) / lmrDivisor);
        }
    }
}

Reduction_t GetReduction(Depth_t depth, MoveCount_t moveCount) {
    return reductionTable[depth][moveCount];
}