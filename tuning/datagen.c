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
#include "evaluation.h"

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

static void UpdateContainer(PositionDataContainer_t* container, UciApplicationData_t* data) {
    EvalScore_t staticEval = ScoreOfPosition(&data->boardInfo);
    EvalScore_t qsearchEval = SimpleQsearch(
        &data->boardInfo,
        &data->gameStack,
        &data->zobristStack,
        -INFINITY,
        INFINITY
    );

    if(staticEval == qsearchEval) {
        container->positionData[container->numPositions].boardInfo = data->boardInfo;
        container->numPositions++;
    }
}

static void WriteContainerToFile(
    PositionDataContainer_t* container,
    GameEndStatus_t status,
    Color_t victim,
    FILE* fp
)
{
    DatagenResult_t datagenResult;
    switch (status) {
    case checkmate:
        datagenResult = (victim == black) ? DATAGEN_WIN : DATAGEN_LOSS;
        break;
    case draw: 
        datagenResult = DATAGEN_DRAW;
        break;
    default:
        printf("INVALID CONTAINER WRITE");
        return;
    }

    for(int i = 0; i < container->numPositions; i++) {
        PositionDataEntry_t* entry = &container->positionData[i];
        entry->datagenResult = datagenResult;

        fwrite(entry, sizeof(PositionDataEntry_t), 1, fp);
    }
}

static void GameLoop(UciApplicationData_t* data, FILE* fp) {
    PositionDataContainer_t container;
    ContainerInit(&container);

    while(true) {
        MoveEntryList_t moveList;
        CompleteMovegen(&moveList, &data->boardInfo, &data->gameStack);
        MoveIndex_t maxIndex = moveList.maxIndex;

        GameEndStatus_t gameEndStatus = 
            CurrentGameEndStatus(&data->boardInfo, &data->gameStack, &data->zobristStack, maxIndex);

        if(gameEndStatus != ongoing) {
            WriteContainerToFile(&container, gameEndStatus, data->boardInfo.colorToMove, fp);
            return;
        }

        SearchResults_t searchResults = Search(
            &data->uciSearchInfo,
            &data->boardInfo,
            &data->gameStack,
            &data->zobristStack,
            false
        );

        if(searchResults.score >= MATE_THRESHOLD) {
            // opponent is victim
            Color_t victim = !data->boardInfo.colorToMove;
            WriteContainerToFile(&container, checkmate, victim, fp);
            return;
        } else if(searchResults.score <= -MATE_THRESHOLD) {
            // we are victim
            Color_t victim = data->boardInfo.colorToMove;
            WriteContainerToFile(&container, checkmate, victim, fp);
            return;
        }

        UpdateContainer(&container, data);

        MakeMove(&data->boardInfo, &data->gameStack, searchResults.bestMove);
    }
}

static bool RandomMoves(UciApplicationData_t* data, Generator_t* generator) {
    for(int i = 0; i < RAND_PLY_COUNT; i++) {
        MoveEntryList_t moveList;
        CompleteMovegen(&moveList, &data->boardInfo, &data->gameStack);

        if(moveList.maxIndex == movelist_empty) {
            printf("RandMate");
            return false;
        }

        MoveIndex_t index = RandUnsigned64(generator) % (moveList.maxIndex + 1);

        Move_t move = moveList.moves[index].move;
        MakeMove(&data->boardInfo, &data->gameStack, move);
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