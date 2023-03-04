#include "transposition_table.h"

#include <stdlib.h>
#include <assert.h>

static void InitTableEntries(TranspositionTable_t* table) {
    for(int i = 0; i < table->numEntries; i++) {
        table->entries[i].hash = 0;
    }
}

void TranspositionTableInit(TranspositionTable_t* table, Megabytes_t megabytes) {
    Bytes_t entrySize = sizeof(TTEntry_t);
    Bytes_t bytes = megabytes * bytes_per_megabyte;

    table->numEntries = bytes / entrySize;

    table->entries = malloc(bytes);
    assert(table->entries != NULL);

    InitTableEntries(table);
}

void TeardownTT(TranspositionTable_t* table) {
    free(table->entries);
}