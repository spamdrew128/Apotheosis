#include "transposition_table.h"

#include <stdlib.h>
#include <assert.h>

typedef uint16_t Quality_t;

void ClearTTEntries(TranspositionTable_t* table) {
    for(int i = 0; i < table->numEntries; i++) {
        table->entries[i].flag = tt_uninitialized;
        table->entries[i].key = 0;
        table->entries[i].entryEpoch = 0;
        InitMove(&table->entries[i].bestMove);
    }
}

void TranspositionTableInit(TranspositionTable_t* table, Megabytes_t megabytes) {
    Bytes_t entrySize = sizeof(TTEntry_t);
    Bytes_t bytes = megabytes * bytes_per_megabyte;

    table->tableEpoch = 0;
    table->numEntries = bytes / entrySize;

    table->entries = malloc(table->numEntries * entrySize);
    assert(table->entries != NULL);

    ClearTTEntries(table);
}

TTIndex_t GetTTIndex(TranspositionTable_t* table, ZobristHash_t hash) {
    return hash % table->numEntries;
}

static EvalScore_t ScoreToTT(EvalScore_t score, Ply_t ply) {
    // Adjust to be relative to the node, rather than relative to the position
    if(score >= MATE_THRESHOLD) {
        return score + ply;
    } else if(score <= -MATE_THRESHOLD) {
        return score - ply;
    }

    return score;
}

static EvalScore_t ScoreFromTT(EvalScore_t score, Ply_t ply) {
    if(score >= MATE_THRESHOLD) {
        return score - ply;
    } else if(score <= -MATE_THRESHOLD) {
        return score + ply;
    }

    return score;
}

TTEntry_t GetTTEntry(TranspositionTable_t* table, TTIndex_t index, Ply_t ply) {
    TTEntry_t entry = table->entries[index];
    entry.bestScore = ScoreFromTT(entry.bestScore, ply);
    return entry;
}

bool TTHit(TTEntry_t entry, ZobristHash_t hash) {
    return (entry.key == (TTKey_t)hash) && (entry.flag != tt_uninitialized);
}

TTFlag_t DetermineTTFlag(EvalScore_t bestScore, EvalScore_t oldAlpha, EvalScore_t alpha, EvalScore_t beta) {
    if(bestScore >= beta) {
        return lower_bound;
    } else if(alpha != oldAlpha) {
        return exact;
    } else {
        return upper_bound;
    }
}

bool ShouldReplace(TranspositionTable_t* table, TTEntry_t oldEntry, Depth_t newDepth, TTFlag_t newFlag) {
    if(newFlag == exact) {
        return true;
    }

    // replacement formula borrowed from Svart: https://github.com/crippa1337/svart/blob/master/src/engine/tt.rs
    Quality_t oldQuality = oldEntry.entryEpoch * 2 + oldEntry.depth;
    Quality_t newQuality = table->tableEpoch * 2 + newDepth;

    return newQuality >= oldQuality;
}

void StoreTTEntry(
    TranspositionTable_t* table,
    TTIndex_t index,
    TTFlag_t flag,
    Depth_t depth,
    Ply_t ply,
    Move_t bestMove,
    EvalScore_t bestScore,
    ZobristHash_t hash
)
{
    table->entries[index].flag = flag;
    table->entries[index].depth = depth;
    table->entries[index].entryEpoch = table->tableEpoch;
    table->entries[index].bestMove = bestMove;
    table->entries[index].bestScore = ScoreToTT(bestScore, ply);
    table->entries[index].key = (TTKey_t)hash;
}

bool TTCutoffIsPossible(TTEntry_t entry, EvalScore_t alpha, EvalScore_t beta, Depth_t currentDepth) {
    if(entry.depth < currentDepth) {
        return false;
    }

    switch(entry.flag) {
        case exact:
            return true;
        case lower_bound:
            return entry.bestScore >= beta;
        case upper_bound:
            return entry.bestScore <= alpha;
        default:
            return false;
    }
}

HashFull_t HashFull(TranspositionTable_t* table) {
    assert(table->numEntries >= 1000);

    HashFull_t hashFull = 0;
    for(TTIndex_t i = 0; i < 1000; i++) {
        if(table->entries[i].flag != tt_uninitialized) {
            hashFull++;
        }
    }

    return hashFull;
}

void AgeTranspositionTable(TranspositionTable_t* table) {
    table->tableEpoch++;
}

void TeardownTT(TranspositionTable_t* table) {
    free(table->entries);
    table->entries = NULL;
}