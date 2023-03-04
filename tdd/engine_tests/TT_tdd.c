#include "TT_tdd.h"
#include "debug.h"

enum {
    some_tt_size = 16
};

static void ShouldInitToCorrectSize() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    TTLength_t expectedEntryCount = some_tt_size*bytes_per_megabyte / sizeof(*table.entries);

    PrintResults(table.numEntries == expectedEntryCount);

    TeardownTT(&table);
}

void TranspositionTableTDDRunner() {
    ShouldInitToCorrectSize();
}