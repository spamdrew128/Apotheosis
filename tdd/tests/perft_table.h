#ifndef __PERFT_TABLE_H__
#define __PERFT_TABLE_H__

#include <stdbool.h>

#include "board_constants.h"
#include "FEN.h"
#include "perft_table_entries.h"

typedef unsigned long long PerftCount_t;

typedef struct {
    FEN_t fen;
    PerftCount_t expectedCounts[6];
} PerftTestContainer_t;

void RunAllPerftTests(bool shouldRun);

#endif