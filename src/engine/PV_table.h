#ifndef __PV_TABLE_H__
#define __PV_TABLE_H__

#include <stdint.h>

#include "move.h"
#include "chess_search.h"
#include "engine_types.h"

typedef uint8_t PvLength_t;

typedef struct {
    Move_t moveMatrix[PLY_MAX][PLY_MAX];
    PvLength_t pvLength [PLY_MAX+1];
} PvTable_t;

void PvLengthInit(PvTable_t* pvTable, Ply_t ply);

void UpdatePvTable(PvTable_t* pvTable, Move_t move, Ply_t ply);

Move_t PvTableBestMove(PvTable_t* pvTable);

#endif