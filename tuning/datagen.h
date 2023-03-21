#ifndef __DATAGEN_H__
#define __DATAGEN_H__

#include "board_info.h"
#include "PST.h"

typedef double PositionResult_t;
#define POSITION_WIN (PositionResult_t) 1
#define POSITION_DRAW (PositionResult_t) 0.5
#define POSITION_LOSS (PositionResult_t) 0

typedef struct {
    Phase_t phase[NUM_PHASES];

    int pieceCount[NUM_PIECES];
    Bitboard_t all[2];
    Bitboard_t pieceBBs[NUM_PIECES];
    PositionResult_t positionResult;
} TEntry_t;

typedef struct {
    TEntry_t entryList[2048];
    int numPositions;
} TuningDatagenContainer_t;

void GenerateData(const char* filename);

void FillTEntry(TEntry_t* tEntry, BoardInfo_t* boardInfo);

#endif