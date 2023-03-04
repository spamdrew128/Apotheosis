#include "TT_tdd.h"
#include "debug.h"

enum {
    some_tt_size = 16,
    some_zobrist_hash = 830928908
};

static void ShouldInitToCorrectSize() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    TTLength_t expectedEntryCount = some_tt_size*bytes_per_megabyte / sizeof(*table.entries);

    PrintResults(table.numEntries == expectedEntryCount);

    TeardownTT(&table);
}

static void ShoulNotHitWhenUninitialized() {
    TTEntry_t entry;
    entry.flag = uninitialized_flag;
    entry.hash = some_zobrist_hash;

    bool hit = TTHit(&entry, some_zobrist_hash);
    PrintResults(!hit);
}

static void ShoulNotHitWithDifferentHashs() {
    TTEntry_t entry;
    entry.flag = exact;
    entry.hash = some_zobrist_hash - 1;

    bool hit = TTHit(&entry, some_zobrist_hash);
    PrintResults(!hit);
}

static void ShouldNotCutoffIfLowerDepth() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    

    TeardownTT(&table);
}

void TranspositionTableTDDRunner() {
    ShouldInitToCorrectSize();
    ShoulNotHitWhenUninitialized();
    ShoulNotHitWithDifferentHashs();

    ShouldNotCutoffIfLowerDepth();
}