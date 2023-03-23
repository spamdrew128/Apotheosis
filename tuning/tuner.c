#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "tuner.h"
#include "board_constants.h"
#include "datagen.h"
#include "bitboards.h"
#include "eval_constants.h"
#include "PST.h"
#include "util_macros.h"

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
    Phase_t phase;
    
    Bitboard_t all[2];
    Bitboard_t pieceBBs[NUM_PIECES];
    double result;
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

    tEntry->phase = MIN(midgame_phase, PHASE_MAX);
}

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

    Phase_t mgPhase = entry.phase;
    Phase_t egPhase = PHASE_MAX - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / PHASE_MAX;
}

static double Sigmoid(double E) {
    return 1 / (1 + exp(-K * E));
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

    double cost = Cost(&tuningData);
    double meanSquaredError = MSE(&tuningData, cost);
    printf("Cost %f\n", cost);
    printf("MSE %f\n", meanSquaredError);
}