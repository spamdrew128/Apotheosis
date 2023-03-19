#ifndef __DATAGEN_H__
#define __DATAGEN_H__

#include "board_info.h"

typedef double DatagenResult_t;
#define DATAGEN_WIN (DatagenResult_t) 1
#define DATAGEN_DRAW (DatagenResult_t) 0.5
#define DATAGEN_LOSS (DatagenResult_t) 0

typedef struct {
    BoardInfo_t boardInfo;
    DatagenResult_t datagenResult;
} PositionDataEntry_t;

void GenerateData(const char* filename);

#endif