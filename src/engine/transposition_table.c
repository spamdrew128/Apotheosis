#include "transposition_table.h"

#include <stdlib.h>
#include <assert.h>

static void UninitializeTableEntries(TranspositionTable_t* table) {
    for(int i = 0; i < table->numEntries; i++) {
        table->entries[i].flag = uninitialized_flag;
        table->entries[i].hash = 0;
    }
}

void TranspositionTableInit(TranspositionTable_t* table, Megabytes_t megabytes) {
    Bytes_t entrySize = sizeof(TTEntry_t);
    Bytes_t bytes = megabytes * bytes_per_megabyte;

    table->numEntries = bytes / entrySize;

    table->entries = malloc(bytes);
    assert(table->entries != NULL);

    UninitializeTableEntries(table);
}

TTEntry_t* GetTTEntry(TranspositionTable_t* table, ZobristHash_t hash) {
    return &table->entries[hash % table->numEntries];
}

bool TTHit(TTEntry_t* entry, ZobristHash_t hash) {
    return (entry->hash == hash) && (entry->flag != uninitialized_flag);
}

void ReplaceTTEntry(
    TTEntry_t* entry,
    TTFlag_t flag,
    Depth_t depth,
    Move_t move,
    EvalScore_t score,
    ZobristHash_t hash
)
{
    entry->flag = flag;
    entry->depth = depth;
    entry->move = move;
    entry->score = score;
    entry->hash = hash;
}

bool TTCutoffIsPossible(TTEntry_t* entry, EvalScore_t alpha, EvalScore_t beta, Depth_t currentDepth) {
    if(entry->depth < currentDepth) {
        return false;
    }

    switch(entry->flag) {
        case exact:
            return true;
        case lower_bound:
            return entry->score >= beta;
        case upper_bound:
            return entry->score <= alpha;
        default:
            return false;
    }
}

void TeardownTT(TranspositionTable_t* table) {
    free(table->entries);
}