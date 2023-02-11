#ifndef __ZOBRIST_H__
#define __ZOBRIST_H__

#include <stdint.h>
#include "board_constants.h"

#define ZOBRIST_STACK_MAX 1024

typedef uint64_t ZobristHash_t;

typedef struct {
    ZobristHash_t entries[ZOBRIST_STACK_MAX];
    ZobristHash_t num_entries;
} ZobristStack_t;

void InitZobristGenerator();

void InitZobristStack(ZobristStack_t* zobristStack);

#endif