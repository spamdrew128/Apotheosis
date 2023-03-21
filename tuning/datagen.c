#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    TIME_PER_MOVE = 50,
    RAND_PLY_BASE = 7,
};

// data for my curious mind
static int winCount = 0;
static int lossCount = 0;
static int drawCount = 0;
static int highestPly = 0;
static int gameCount = 0;
static long long storedPositions = 0;

static void ContainerInit(TuningDatagenContainer_t* container) {
    container->numPositions = 0;
}

void FillTEntry(TEntry_t* tEntry, BoardInfo_t* boardInfo) {
    tEntry->pieceCount[knight] = empty_set;
    tEntry->pieceCount[bishop] = empty_set;
    tEntry->pieceCount[rook] = empty_set;
    tEntry->pieceCount[pawn] = empty_set;
    tEntry->pieceCount[queen] = empty_set;
    tEntry->pieceCount[king] = empty_set;

    for(int c = 0; c < 2; c++) {
        tEntry->all[c] = boardInfo->allPieces[c];
        tEntry->pieceCount[knight]  |= boardInfo->knights[c];
        tEntry->pieceCount[bishop] |= boardInfo->bishops[c];
        tEntry->pieceCount[rook] |= boardInfo->rooks[c];
        tEntry->pieceCount[pawn] |= boardInfo->pawns[c];
        tEntry->pieceCount[queen] |= boardInfo->queens[c];
        tEntry->pieceCount[king] |= boardInfo->kings[c];
    }

    for(Piece_t p = 0; p < NUM_PIECES; p++) {
        tEntry->pieceCount[p] = PopCount(tEntry->pieceCount[p]);
    }

    Phase_t midgame_phase = 
        tEntry->pieceCount[knight]*KNIGHT_PHASE_VALUE +
        tEntry->pieceCount[bishop]*BISHOP_PHASE_VALUE +
        tEntry->pieceCount[rook]*ROOK_PHASE_VALUE +
        tEntry->pieceCount[queen]*QUEEN_PHASE_VALUE;

    tEntry->phase[mg_phase] = MIN(midgame_phase, PHASE_MAX);
    tEntry->phase[eg_phase] = PHASE_MAX - midgame_phase;
}

static void UpdateContainer(TuningDatagenContainer_t* container, UciApplicationData_t* data) {
    EvalScore_t staticEval = ScoreOfPosition(&data->boardInfo);
    EvalScore_t qsearchEval = SimpleQsearch(
        &data->boardInfo,
        &data->gameStack,
        &data->zobristStack,
        -INFINITY,
        INFINITY
    );

    if(staticEval == qsearchEval) {
        FillTEntry(&container->entryList[container->numPositions], &data->boardInfo);
        container->numPositions++;
        storedPositions++;
    }
}

static void WriteContainerToFile(
    TuningDatagenContainer_t* container,
    GameEndStatus_t status,
    Color_t victim,
    FILE* fp
)
{
    PositionResult_t positionResult;
    switch (status) {
    case checkmate:
        positionResult = (victim == black) ? POSITION_WIN : POSITION_LOSS;
        break;
    case draw: 
        positionResult = POSITION_DRAW;
        break;
    default:
        printf("INVALID CONTAINER WRITE");
        return;
    }

    for(int i = 0; i < container->numPositions; i++) {
        TEntry_t* entry = &container->entryList[i];
        entry->positionResult = positionResult;

        fwrite(entry, sizeof(*entry), 1, fp);
    }

    if(positionResult == POSITION_WIN) {
        winCount++;
    } else if(positionResult == POSITION_LOSS) {
        lossCount++;
    } else {
        drawCount++;
    }

    highestPly = MAX(container->numPositions, highestPly);
    gameCount++;
}

static void GameLoop(UciApplicationData_t* data, FILE* fp) {
    TuningDatagenContainer_t container;
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

static bool RandomMoves(UciApplicationData_t* data, Generator_t* generator, Ply_t randPlyCount) {
    for(int i = 0; i < randPlyCount; i++) {
        MoveEntryList_t moveList;
        CompleteMovegen(&moveList, &data->boardInfo, &data->gameStack);

        if(moveList.maxIndex == movelist_empty) {
            printf("RandMate\n");
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

    int percentComplete = -1;
    for(int i = 0; i < NUM_GAMES; i++) {
        UciSearchInfo_t* searchInfo;

        Ply_t randPlyCount = RAND_PLY_BASE + (i%2);

        bool usablePosition = false;
        while(!usablePosition) {
            UciApplicationDataInit(&data);

            searchInfo = &data.uciSearchInfo;
            searchInfo->overhead = 0;
            searchInfo->forceTime = TIME_PER_MOVE;

            usablePosition = RandomMoves(&data, &generator, randPlyCount); // returns false if we hit a mate
        }

        GameLoop(&data, fp);

        TeardownTT(&searchInfo->tt);

        if((i+1)*100 / NUM_GAMES > percentComplete) {
            percentComplete = (i+1)*100 / NUM_GAMES;

            printf("%d%% complete\n", percentComplete);
            printf("W: %d L: %d D: %d\n", winCount, lossCount, drawCount);
            printf("Game Count: %d\n", gameCount);
            printf("Stored Positions: %lld\n", storedPositions);
            printf("Longest game: %d moves\n\n", highestPly+8);
        }
    }

    fclose(fp);
}