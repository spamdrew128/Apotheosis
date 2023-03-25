#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "tuner.h"
#include "board_constants.h"
#include "datagen.h"
#include "bitboards.h"
#include "eval_constants.h"
#include "PST.h"
#include "util_macros.h"

#define LINE_BUFFER 500

typedef double Weight_t;
Weight_t PSTWeights[NUM_PHASES][NUM_PIECES][NUM_SQUARES] = { // PST from black perspective, mirror if white
    { KNIGHT_MG_PST, BISHOP_MG_PST, ROOK_MG_PST, QUEEN_MG_PST, PAWN_MG_PST, KING_MG_PST },
    { KNIGHT_EG_PST, BISHOP_EG_PST, ROOK_EG_PST, QUEEN_EG_PST, PAWN_EG_PST, KING_EG_PST },
};

Weight_t MaterialWeights[NUM_PHASES][NUM_PIECES] = {
    { knight_value, bishop_value, rook_value, queen_value, pawn_value, king_value },
    { knight_value, bishop_value, rook_value, queen_value, pawn_value, king_value },
};

double K = 0.0075;

typedef struct {
    double phaseConstant[NUM_PHASES];
    double result;

    Bitboard_t all[2];
    Bitboard_t pieceBBs[NUM_PIECES];
} TEntry_t;

typedef struct {
    TEntry_t* entryList;
    int numEntries;
} TuningData_t;

void FillTEntry(TEntry_t* tEntry, BoardInfo_t* boardInfo) {
    tEntry->pieceBBs[knight] = empty_set;
    tEntry->pieceBBs[bishop] = empty_set;
    tEntry->pieceBBs[rook] = empty_set;
    tEntry->pieceBBs[pawn] = empty_set;
    tEntry->pieceBBs[queen] = empty_set;
    tEntry->pieceBBs[king] = empty_set;

    for(int c = 0; c < 2; c++) {
        tEntry->all[c] = boardInfo->allPieces[c];
        tEntry->pieceBBs[knight] |= boardInfo->knights[c];
        tEntry->pieceBBs[bishop] |= boardInfo->bishops[c];
        tEntry->pieceBBs[rook] |= boardInfo->rooks[c];
        tEntry->pieceBBs[pawn] |= boardInfo->pawns[c];
        tEntry->pieceBBs[queen] |= boardInfo->queens[c];
        tEntry->pieceBBs[king] |= boardInfo->kings[c];
    }

    Phase_t midgame_phase = 
        PopCount(tEntry->pieceBBs[knight])*KNIGHT_PHASE_VALUE +
        PopCount(tEntry->pieceBBs[bishop])*BISHOP_PHASE_VALUE +
        PopCount(tEntry->pieceBBs[rook])*ROOK_PHASE_VALUE +
        PopCount(tEntry->pieceBBs[queen])*QUEEN_PHASE_VALUE;

    midgame_phase = MIN(midgame_phase, PHASE_MAX);

    tEntry->phaseConstant[mg_phase] = (double)midgame_phase / PHASE_MAX;
    tEntry->phaseConstant[eg_phase] = 1 - tEntry->phaseConstant[mg_phase];
}

static int EntriesInFile(FILE* fp) {
    uint64_t lines = 0;

    char buffer[LINE_BUFFER];
    while(fgets(buffer, LINE_BUFFER, fp)) {
        lines++;
    }

    rewind(fp);

    return lines;
}

static void TuningDataInit(TuningData_t* tuningData, const char* filename) {
    FILE* fp = fopen(filename, "rb");
    
    tuningData->numEntries = EntriesInFile(fp);
    tuningData->entryList = malloc(tuningData->numEntries * sizeof(TEntry_t));

    char buffer[LINE_BUFFER];
    for(int i = 0; i < tuningData->numEntries; i++) {
        fgets(buffer, LINE_BUFFER, fp);

        assert(strlen(buffer) > 2);
        int lineIndex = 0;
        while(buffer[lineIndex] != '"') {
            lineIndex++; 
        }

        char fenBuffer[FEN_BUFFER_SIZE];
        memset(fenBuffer, '\0', FEN_BUFFER_SIZE);
        memcpy(fenBuffer, buffer, lineIndex * sizeof(char));

        BoardInfo_t boardInfo;
        GameStack_t gameStack;
        ZobristStack_t zobristStack;
        InterpretFEN(fenBuffer, &boardInfo, &gameStack, &zobristStack);

        FillTEntry(&tuningData->entryList[i], &boardInfo);

        lineIndex++; 
        if(buffer[lineIndex] == '1') {
            if(buffer[lineIndex + 1] == '-') {
                tuningData->entryList[i].result = 1;
            } else {
                tuningData->entryList[i].result = 0.5;
            }
        } else {
            assert(buffer[lineIndex] == '0');
            tuningData->entryList[i].result = 0;
        }
    }

    fclose(fp);
}

static void MaterialAndPSTComponent(
    TEntry_t entry,
    double* mgScore,
    double* egScore
)
{
    for(Piece_t p = 0; p < NUM_PIECES; p++) {
        Bitboard_t whitePieces = entry.pieceBBs[p] & entry.all[white];
        Bitboard_t blackPieces = entry.pieceBBs[p] & entry.all[black];

        int whiteCount = PopCount(whitePieces);
        *mgScore += whiteCount * MaterialWeights[mg_phase][p];
        *egScore += whiteCount * MaterialWeights[eg_phase][p];
        
        int blackCount = PopCount(blackPieces);
        *mgScore -= blackCount * MaterialWeights[mg_phase][p];
        *egScore -= blackCount * MaterialWeights[eg_phase][p];

        while(whitePieces) {
            Square_t sq = MIRROR(LSB(whitePieces));
            *mgScore += PSTWeights[mg_phase][p][sq];
            *egScore += PSTWeights[eg_phase][p][sq];
            ResetLSB(&whitePieces);
        }
        while(blackPieces) {
            Square_t sq = LSB(blackPieces);
            *mgScore -= PSTWeights[mg_phase][p][sq];
            *egScore -= PSTWeights[eg_phase][p][sq];
            ResetLSB(&blackPieces);
        }
    }
}

static double Evaluation(TEntry_t entry) {
    double mgScore = 0;
    double egScore = 0;

    MaterialAndPSTComponent(entry, &mgScore, &egScore);

    return (mgScore * entry.phaseConstant[mg_phase] + egScore * entry.phaseConstant[eg_phase]);
}

static double Sigmoid(double E) {
    return 1 / (1 + exp(-K * E));
}

static double SigmoidPrime(double sigmoid) {
    return K * sigmoid * (1 - sigmoid);
}

static double Cost(TuningData_t* tuningData) {
    double totalError = 0;
    for(int i = 0; i < tuningData->numEntries; i++) {
        TEntry_t entry = tuningData->entryList[i];
        double E = Evaluation(entry);

        double error = entry.result - Sigmoid(E);
        totalError += error * error;
    }

    return totalError;
}

static double MSE(TuningData_t* tuningData, double cost) {
    return cost / tuningData->numEntries;
}

void TuneParameters(const char* filename) {
    TuningData_t tuningData;
    TuningDataInit(&tuningData, filename);

    // double cost = Cost(&tuningData);
    // double meanSquaredError = MSE(&tuningData, cost);
    // printf("Cost %f\n", cost);
    // printf("MSE %f\n", meanSquaredError);

    free(tuningData.entryList);
}