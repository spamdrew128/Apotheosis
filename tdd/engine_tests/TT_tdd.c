#include "TT_tdd.h"

enum {
    some_tt_size = 16
};

static void ShouldInitToCorrectSize() {
    TranspositionTable_t table;
    TranspositionTableInit(&table, some_tt_size);

    

    TeardownTT(&table);
}

void TranspositionTableTDDRunner() {
    ShouldInitToCorrectSize();
}