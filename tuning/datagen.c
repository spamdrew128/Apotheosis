#include <stdio.h>
#include <stdlib.h>

#include "datagen.h"
#include "uci.h"
#include "RNG.h"
#include "endings.h"
#include "FEN.h"

enum {
    NUM_GAMES = 10000,
    TIME_PER_MOVE = 100,
    RAND_PLY_COUNT = 8,
};

static void GameLoop(UciSearchInfo_t* searchInfo) {

}

static void RandomMoves(UciSearchInfo_t* searchInfo, Generator_t* generator) {
    for(int i = 0; i < RAND_PLY_COUNT; i++) {

    }
}

void GenerateData(const char* filename) {
    FILE* fp = fopen(filename, "wb");

    UciApplicationData_t data;

    Generator_t generator;
    InitRNG(&generator, false);

    for(int i = 0; i < NUM_GAMES; i++) {
        UciApplicationDataInit(&data);

        UciSearchInfo_t* searchInfo = &data.uciSearchInfo;
        searchInfo->overhead = 0;
        searchInfo->forceTime = TIME_PER_MOVE;

        RandomMoves(&searchInfo, &generator);

        GameLoop(&searchInfo);

        TeardownTT(&searchInfo->tt);
    }

    fclose(fp);
}