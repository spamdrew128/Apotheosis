#include "TT_tdd.h"
#include "debug.h"

enum {
    some_flag = 0,
    some_tt_size = 16,
    some_zobrist_hash = 830928908,
    some_alpha = 2,
    some_beta = 4,
    some_depth = 3,
};

static void ShouldInitToCorrectSize() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    TTLength_t expectedEntryCount = some_tt_size*bytes_per_megabyte / sizeof(*table.entries);

    PrintResults(table.numEntries == expectedEntryCount);

    TeardownTT(&table);
}

static void ShouldNotHitWhenUninitialized() {
    TTEntry_t entry;
    entry.flag = some_flag;
    entry.hash = 0;

    bool hit = TTHit(entry, some_zobrist_hash);
    PrintResults(!hit);
}

static void ShouldNotHitWithDifferentHashs() {
    TTEntry_t entry;
    entry.flag = exact;
    entry.hash = some_zobrist_hash - 1;

    bool hit = TTHit(entry, some_zobrist_hash);
    PrintResults(hit == false);
}

static void ShouldHitWithSameHashs() {
    TTEntry_t entry;
    entry.flag = exact;
    entry.hash = some_zobrist_hash;

    bool hit = TTHit(entry, some_zobrist_hash);
    PrintResults(hit == true);
}

static void ShouldNotCutoffIfLowerDepth() {
    TTEntry_t entry;
    entry.depth = some_depth;

    bool cutoff = TTCutoffIsPossible(entry, some_alpha, some_beta, entry.depth + 1);
    PrintResults(cutoff == false);
}

static void ShouldCutoffIfScoreIsExact() {
    TTEntry_t entry;
    entry.depth = some_depth;
    entry.flag = exact;

    bool cutoff = TTCutoffIsPossible(entry, some_alpha, some_beta, entry.depth);
    PrintResults(cutoff == true);
}

static void ShouldCutoffIfScoreIsLowerBoundAndGreaterThanBeta() {
    TTEntry_t entry;
    entry.depth = some_depth;
    entry.flag = lower_bound;
    entry.bestScore = some_beta + 1;

    bool cutoff = TTCutoffIsPossible(entry, some_alpha, some_beta, entry.depth);
    PrintResults(cutoff == true);
}

static void ShouldCutoffIfScoreIsUpperBoundAndLessThanAlpha() {
    TTEntry_t entry;
    entry.depth = some_depth;
    entry.flag = upper_bound;
    entry.bestScore = some_alpha - 1;

    bool cutoff = TTCutoffIsPossible(entry, some_alpha, some_beta, entry.depth);
    PrintResults(cutoff == true);
}

void TranspositionTableTDDRunner() {
    ShouldInitToCorrectSize();
    ShouldNotHitWhenUninitialized();
    ShouldNotHitWithDifferentHashs();
    ShouldHitWithSameHashs();

    ShouldNotCutoffIfLowerDepth();
    ShouldCutoffIfScoreIsExact();
    ShouldCutoffIfScoreIsLowerBoundAndGreaterThanBeta();
    ShouldCutoffIfScoreIsUpperBoundAndLessThanAlpha();
}