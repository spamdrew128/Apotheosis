#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tuner.h"
#include "board_constants.h"
#include "datagen.h"
#include "bitboards.h"
#include "evaluation.h"
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

static void MaterialAndPSTComponent(
    TEntry_t entry,
    double* mgScore,
    double* egScore
)
{
    for(Piece_t p = 0; p < NUM_PIECES; p++) {
        Bitboard_t whitePieces = entry.pieceBBs[p] & entry.all[white];
        Bitboard_t blackPieces = entry.pieceBBs[p] & entry.all[black];

        *mgScore += entry.pieceCount[white][p] * MaterialWeights[mg_phase][p];
        *egScore += entry.pieceCount[white][p] * MaterialWeights[eg_phase][p];
        
        *mgScore -= entry.pieceCount[black][p] * MaterialWeights[mg_phase][p];
        *egScore -= entry.pieceCount[black][p] * MaterialWeights[eg_phase][p];

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

static double Sigmoid(double n) {
    
}

static double Cost(TuningData_t* tuningData) {
    double totalError = 0;
    for(int i = 0; i < tuningData->numEntries; i++) {
        TEntry_t entry = tuningData->entryList[i];
        double eval = Evaluation(entry);

        double error = entry.positionResult - Evaluation(entry);
        totalError += error * error;
    }
}

void TuneParameters(const char* filename) {
    TuningData_t tuningData;
    TuningDataInit(&tuningData, filename);



    printf(filename);
}