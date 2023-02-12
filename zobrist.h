#ifndef __ZOBRIST_H__
#define __ZOBRIST_H__

#include <stdint.h>
#include "board_constants.h"
#include "game_state.h"
#include "board_info.h"

#define ZOBRIST_STACK_MAX 1024

enum {
    zobrist_stack_empty
};

typedef uint64_t ZobristHash_t;

typedef struct {
    ZobristHash_t entries[ZOBRIST_STACK_MAX];
    int maxIndex;
} ZobristStack_t;

void InitZobristGenerator();

void InitZobristStack(ZobristStack_t* zobristStack);

ZobristHash_t HashPosition(BoardInfo_t* boardInfo, GameState_t* gameState, Color_t colorToMove);

void AddZobristHashToStack(ZobristStack_t* zobristStack, ZobristHash_t hash);

void RemoveZobristHashFromStack(ZobristStack_t* zobristStack);

#endif