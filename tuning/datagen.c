#include <stdio.h>
#include <stdlib.h>

#include "datagen.h"
#include "uci.h"
#include "endings.h"

enum {
    NUM_GAMES = 10000,
    TIME_PER_MOVE = 100,
};

static void GameLoop(UciSearchInfo_t* searchInfo) {
    
}

static void RandomMoves(UciSearchInfo_t* searchInfo) {

}

void GenerateData(const char* filename) {
    FILE* fp = fopen(filename, "wb");

    UciSearchInfo_t searchInfo;
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;

    for(int i = 0; i < NUM_GAMES; i++) {
        UciSearchInfoInit(&searchInfo);
        searchInfo.overhead = 0;
        searchInfo.forceTime = TIME_PER_MOVE;

        RandomMoves(&searchInfo);

        GameLoop(&searchInfo);

        TeardownTT(&searchInfo.tt);
    }

    fclose(fp);
}