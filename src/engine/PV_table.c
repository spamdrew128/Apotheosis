#include <stdlib.h>
#include <assert.h>

#include "PV_table.h"

void PvTableInit(PvTable_t* pvTable, Depth_t maxDepth) {
    pvTable->moveMatrix = malloc(maxDepth * sizeof(*pvTable->moveMatrix));
    assert(pvTable->moveMatrix != NULL);

    for(int i = 0; i < maxDepth; i++) {
        Move_t* moveArray = malloc(maxDepth * sizeof(*moveArray));
        assert(moveArray != NULL);

        pvTable->moveMatrix[i] = moveArray;
    }

    pvTable->maxPly = (Ply_t)maxDepth;
}

void PvTableTeardown(PvTable_t* pvTable) {
    for(int i = 0; i < pvTable->maxPly; i++) {
        free(pvTable->moveMatrix[i]);
    }

    free(pvTable->moveMatrix);
}

void UpdatePvTable(PvTable_t* pvTable, Move_t move, Ply_t ply) {
    pvTable->moveMatrix[ply][ply] = move;

    for(Ply_t currentPly = ply + 1; currentPly < pvTable->maxPly; currentPly++) {
        pvTable->moveMatrix[ply][currentPly] = pvTable->moveMatrix[ply+1][currentPly];
    }
}