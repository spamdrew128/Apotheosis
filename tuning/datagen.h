#ifndef __DATAGEN_H__
#define __DATAGEN_H__

#include "board_info.h"

typedef struct {
    BoardInfo_t boardInfo;
    GameEndStatus_t result;
} PositionDataEntry_t;

void GenerateData(const char* filename);

#endif