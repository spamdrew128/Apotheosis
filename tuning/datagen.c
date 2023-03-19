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
#include "chess_search.h"

enum {
    NUM_GAMES = 10000,
    TIME_PER_MOVE = 100,
    RAND_PLY_COUNT = 8,
};

typedef struct {
    PositionDataEntry_t positionData[2048];
    int numPositions;
} PositionDataContainer_t;


static void ContainerInit(PositionDataContainer_t* container) {
    container->numPositions = 0;
}

static void UpdateContainer(PositionDataContainer_t* container, BoardInfo_t* boardInfo) {
    container->positionData[container->numPositions].boardInfo = *boardInfo;
    container->numPositions++;
}

static void GameLoop(UciApplicationData_t* data, FILE* fp) {
    bool running = true;
    GameEndStatus_t gameEndStatus;
    PositionDataContainer_t container;
    ContainerInit(&container);

    while(running) {
        MoveEntryList_t moveList;
        CompleteMovegen(&moveList, &data->boardInfo, &data->gameStack);
        MoveIndex_t maxIndex = moveList.maxIndex;

        SearchResults_t searchResults = Search(
            &data->uciSearchInfo,
            &data->boardInfo,
            &data->gameStack,
            &data->zobristStack,
            false
        );

        MakeMove(&data->boardInfo, &data->gameStack, searchResults.bestMove);

        gameEndStatus = 
            CurrentGameEndStatus(&data->boardInfo, &data->gameStack, &data->zobristStack, maxIndex);

        running = gameEndStatus == ongoing;
    }
}

static bool RandomMoves(UciApplicationData_t* data, Generator_t* generator) {
    for(int i = 0; i < RAND_PLY_COUNT; i++) {
        MoveEntryList_t moveList;
        CompleteMovegen(&moveList, &data->boardInfo, &data->gameStack);

        MoveIndex_t index = RandUnsigned64(generator) % (moveList.maxIndex + 1);

        Move_t move = moveList.moves[index].move;
        MakeMove(&data->boardInfo, &data->gameStack, move);

        if(moveList.maxIndex == movelist_empty) {
            printf("RandMate");
            return false;
        }
    }

    return true;
}

void GenerateData(const char* filename) {
    FILE* fp = fopen(filename, "wb");

    UciApplicationData_t data;

    Generator_t generator;
    InitRNG(&generator, false);

    for(int i = 0; i < NUM_GAMES; i++) {
        UciSearchInfo_t* searchInfo;

        bool usablePosition = false;
        while(!usablePosition) {
            UciApplicationDataInit(&data);

            searchInfo = &data.uciSearchInfo;
            searchInfo->overhead = 0;
            searchInfo->forceTime = TIME_PER_MOVE;

            usablePosition = RandomMoves(&data, &generator); // returns false if we hit a mate
        }

        GameLoop(&data, fp);

        TeardownTT(&searchInfo->tt);
    }

    fclose(fp);
}