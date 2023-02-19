#include "PV_table_tdd.h"

static PvTable_t pvTable;

void PvTableTDDRunner() {
    PvTableInit(&pvTable, 3);
    PvTableTeardown(&pvTable, 3);
}