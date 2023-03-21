#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tuner.h"
#include "board_constants.h"
#include "datagen.h"
#include "bitboards.h"
#include "evaluation.h"
#include "PST.h"

typedef double Weight_t;
Weight_t PSTWeights[NUM_PHASES][NUM_PIECES][NUM_SQUARES] = { 
    { KNIGHT_MG_PST, BISHOP_MG_PST, ROOK_MG_PST, QUEEN_MG_PST, PAWN_MG_PST, KING_MG_PST },
    { KNIGHT_EG_PST, BISHOP_EG_PST, ROOK_EG_PST, QUEEN_EG_PST, PAWN_EG_PST, KING_EG_PST },
};

Weight_t MaterialWeights[NUM_PHASES][NUM_PIECES - 1] = {
    { knight_value, bishop_value, rook_value, queen_value, pawn_value },
    { knight_value, bishop_value, rook_value, queen_value, pawn_value },
};

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

static double Evalutation(TEntry_t entry) {

}

void TuneParameters(const char* filename) {
    TuningData_t tuningData;
    TuningDataInit(&tuningData, filename);



    printf(filename);
}