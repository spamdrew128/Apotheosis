#include <stdlib.h>
#include <assert.h>

#include "PV_table.h"

void PvLengthInit(PvTable_t* pvTable, Ply_t ply) {
    pvTable->pvLength[ply] = ply;
}

void UpdatePvTable(PvTable_t* pvTable, Move_t move, Ply_t ply) {
    pvTable->pvLength[ply] = pvTable->pvLength[ply+1];

    pvTable->moveMatrix[ply][ply] = move;
    for(Ply_t currentPly = ply + 1; currentPly < pvTable->pvLength[ply]; currentPly++) {
        pvTable->moveMatrix[ply][currentPly] = pvTable->moveMatrix[ply+1][currentPly];
    }
}

Move_t PvTableBestMove(PvTable_t* pvTable) {
    return pvTable->moveMatrix[0][0];
}