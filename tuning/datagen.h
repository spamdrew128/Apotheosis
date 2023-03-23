#ifndef __DATAGEN_H__
#define __DATAGEN_H__

#include "board_info.h"
#include "PST.h"
#include "eval_constants.h"
#include "FEN.h"

#define POSITION_WIN "\"1-0\";"
#define POSITION_DRAW "\"1/2-1/2\";"
#define POSITION_LOSS "\"0-1\";"

typedef struct {
    char fenBuffers[2048][FEN_BUFFER_SIZE];
    int numPositions;
} TuningDatagenContainer_t;

void GenerateData(const char* filename);

#endif