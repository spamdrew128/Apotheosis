#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "util_macros.h"
#include "datagen.h"
#include "uci.h"
#include "RNG.h"
#include "endings.h"
#include "FEN.h"
#include "movegen.h"
#include "make_and_unmake.h"

enum {
    NUM_GAMES = 10000,
    TIME_PER_MOVE = 100,
    RAND_PLY_COUNT = 8,
};

static void GameLoop(UciApplicationData_t* data) {

}

static void RandomMoves(UciApplicationData_t* data, Generator_t* generator) {
    for(int i = 0; i < RAND_PLY_COUNT; i++) {
        MoveEntryList_t moveList;
        CompleteMovegen(&moveList, &data->boardInfo, &data->gameStack);

        assert(moveList.maxIndex != movelist_empty);
        int index = RandUnsigned64(generator) % (moveList.maxIndex + 1);

        Move_t move = moveList.moves[index].move;
        MakeMove(&data->boardInfo, &data->gameStack, move);
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

        RandomMoves(&data, &generator);

        GameLoop(&data);

        TeardownTT(&searchInfo->tt);
    }

    fclose(fp);
}