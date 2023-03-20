#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tuner.h"
#include "board_constants.h"
#include "datagen.h"
#include "bitboards.h"

typedef struct {
    TEntry_t* entryList;
    int numEntries;
} TuningData_t;

static int EntriesInFile(FILE* fp) {
    fseek(fp, 0L, SEEK_END);
    size_t bytesInFile = ftell(fp);
    rewind(fp);

    size_t entrySize = sizeof(TEntry_t);
    assert(bytesInFile % entrySize == 0);

    return bytesInFile / entrySize;
}

static void TuningDataInit(TuningData_t* tuningData, const char* filename) {
    FILE* fp = fopen(filename, "rb");

    tuningData->numEntries = EntriesInFile(fp);
    tuningData->entryList = malloc(tuningData->numEntries * sizeof(TEntry_t));

    fread(tuningData->entryList, sizeof(TEntry_t), tuningData->numEntries, fp);

    fclose(fp);
}

void TuneParameters(const char* filename) {
    TuningData_t tuningData;
    TuningDataInit(&tuningData, filename);

    printf(filename);
}