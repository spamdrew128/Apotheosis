#ifndef __TRANSPOSITION_TABLE_H__
#define __TRANSPOSITION_TABLE_H__

#include <stdint.h>
#include <stdbool.h>

#include "move.h"
#include "engine_types.h"
#include "zobrist.h"

typedef uint8_t TTFlag_t;
enum {
    tt_uninitialized,
    lower_bound,
    exact,
    upper_bound
};

typedef uint32_t TTLength_t;
typedef uint32_t TTIndex_t;
typedef uint64_t TTKey_t;
typedef uint16_t HashFull_t;

typedef struct {
    TTFlag_t flag;
    Depth_t depth;
    Move_t bestMove;
    EvalScore_t bestScore;
    TTKey_t key;
} TTEntry_t;

typedef struct {
    TTEntry_t* entries;
    TTLength_t numEntries;
} TranspositionTable_t;


void ClearTTEntries(TranspositionTable_t* table);

void TranspositionTableInit(TranspositionTable_t* table, Megabytes_t megabytes);

TTIndex_t GetTTIndex(TranspositionTable_t* table, ZobristHash_t hash);

TTEntry_t GetTTEntry(TranspositionTable_t* table, TTIndex_t index, Ply_t ply);

bool TTHit(TTEntry_t entry, ZobristHash_t hash);

TTFlag_t DetermineTTFlag(EvalScore_t bestScore, EvalScore_t oldAlpha, EvalScore_t alpha, EvalScore_t beta);

void StoreTTEntry(
    TranspositionTable_t* table,
    TTIndex_t index,
    TTFlag_t flag,
    Depth_t depth,
    Ply_t ply,
    Move_t bestMove,
    EvalScore_t bestScore,
    ZobristHash_t hash
);

bool TTCutoffIsPossible(TTEntry_t entry, EvalScore_t alpha, EvalScore_t beta, Depth_t currentDepth);

HashFull_t HashFull(TranspositionTable_t* table);

void TeardownTT(TranspositionTable_t* table);

#endif