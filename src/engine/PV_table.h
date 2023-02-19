#ifndef __PV_TABLE_H__
#define __PV_TABLE_H__

#include "move.h"
#include "chess_search.h"

typedef struct {
    Move_t** moveMatrix;
} PvTable_t;

void PvTableInit(PvTable_t* pvTable, Depth_t maxDepth);

void PvTableTeardown(PvTable_t* pvTable, Depth_t maxDepth);

#endif