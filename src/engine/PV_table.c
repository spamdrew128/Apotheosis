#include <stdlib.h>
#include <assert.h>

#include "PV_table.h"

void PvTableInit(PvTable_t* pvTable, Depth_t maxDepth) {
    pvTable->moveMatrix = malloc(sizeof(*pvTable->moveMatrix));
    assert(pvTable->moveMatrix != NULL);

    for(int i = 0; i < maxDepth; i++) {
        Move_t* moveArray = malloc(sizeof(*moveArray));
        assert(moveArray != NULL);

        pvTable->moveMatrix[i] = moveArray;
    }
}

void PvTableTeardown(PvTable_t* pvTable, Depth_t maxDepth) {
    for(int i = 0; i < maxDepth; i++) {
        free(pvTable->moveMatrix[i]);
    }

    free(pvTable->moveMatrix);
}