#include "TT_tdd.h"
#include "debug.h"

enum {
    some_flag = 1,
    some_tt_size = 16,
    some_zobrist_hash = 830928908,
    some_alpha = 2,
    some_beta = 4,
    some_depth = 3,
    some_ply = 2,

    some_score = -478,
    m4_score = EVAL_MAX - 4,

    some_epoch = 54,
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
    entry.key = 0;

    bool hit = TTHit(entry, some_zobrist_hash);
    PrintResults(!hit);
}

static void ShouldNotHitWithDifferentHashs() {
    TTEntry_t entry;
    entry.flag = exact;
    entry.key = some_zobrist_hash - 1;

    bool hit = TTHit(entry, some_zobrist_hash);
    PrintResults(hit == false);
}

static void ShouldHitWithSameHashs() {
    TTEntry_t entry;
    entry.flag = exact;
    entry.key = some_zobrist_hash;

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

static void ShouldAdjustMateScores() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    ZobristHash_t hash = some_zobrist_hash;
    TTIndex_t index = GetTTIndex(&table, hash);

    StoreTTEntry(&table, index, some_flag, some_depth, some_ply, NullMove(), m4_score, hash);

    TTEntry_t entry = GetTTEntry(&table, index, some_ply + 3);
    PrintResults(entry.bestScore == m4_score - 3)
}

static void ShouldFavorHigherDepthEntriesOfSameEpoch() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    ZobristHash_t hash = some_zobrist_hash;
    TTIndex_t index = GetTTIndex(&table, hash);

    StoreTTEntry(&table, index, some_flag, some_depth, some_ply, NullMove(), some_score, hash);
    TTEntry_t oldEntry = GetTTEntry(&table, index, some_ply);

    PrintResults(ShouldReplace(&table, oldEntry, some_depth + 1, lower_bound));
}

static void ShouldFavorSlightlyLowerDepthEntriesOfNewerEpoch() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    ZobristHash_t hash = some_zobrist_hash;
    TTIndex_t index = GetTTIndex(&table, hash);

    StoreTTEntry(&table, index, some_flag, some_depth, some_ply, NullMove(), some_score, hash);
    TTEntry_t oldEntry = GetTTEntry(&table, index, some_ply);

    AgeTranspositionTable(&table);
    PrintResults(ShouldReplace(&table, oldEntry, some_depth - 1, lower_bound));
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

    ShouldAdjustMateScores();

    ShouldFavorHigherDepthEntriesOfSameEpoch();
    ShouldFavorSlightlyLowerDepthEntriesOfNewerEpoch();
}