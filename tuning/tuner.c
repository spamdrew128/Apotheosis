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
#include "util_macros.h"

enum {
    pst_offset = 0,
    NUM_FEATURES,

    PST_FEATURE_COUNT = NUM_PIECES * NUM_SQUARES,
};

enum {
    VECTOR_LENGTH = PST_FEATURE_COUNT,
    LINE_BUFFER = 500,
    MAX_EPOCHS = 10000,
};

#define LEARN_RATE 5000

typedef double Gradient_t;
typedef double Weight_t;
Weight_t weights[NUM_PHASES][VECTOR_LENGTH] = {
    { KNIGHT_MG_PST BISHOP_MG_PST ROOK_MG_PST QUEEN_MG_PST PAWN_MG_PST KING_MG_PST },
    { KNIGHT_EG_PST BISHOP_EG_PST ROOK_EG_PST QUEEN_EG_PST PAWN_EG_PST KING_EG_PST },
};

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

static void PSTEval(
    TEntry_t entry,
    double* mgScore,
    double* egScore
)
{
    for(Piece_t p = 0; p < NUM_PIECES; p++) {
        Bitboard_t whitePieces = entry.pieceBBs[p] & entry.all[white];
        Bitboard_t blackPieces = entry.pieceBBs[p] & entry.all[black];

        while(whitePieces) {
            Square_t sq = MIRROR(LSB(whitePieces));
            *mgScore += weights[mg_phase][pst_offset + NUM_SQUARES*p + sq];
            *egScore += weights[eg_phase][pst_offset + NUM_SQUARES*p + sq];
            ResetLSB(&whitePieces);
        }
        while(blackPieces) {
            Square_t sq = LSB(blackPieces);
            *mgScore -= weights[mg_phase][pst_offset + NUM_SQUARES*p + sq];
            *egScore -= weights[eg_phase][pst_offset + NUM_SQUARES*p + sq];
            ResetLSB(&blackPieces);
        }
    }
}

static double Evaluation(TEntry_t entry) {
    double mgScore = 0;
    double egScore = 0;

    PSTEval(entry, &mgScore, &egScore);

    return (mgScore * entry.phaseConstant[mg_phase] + egScore * entry.phaseConstant[eg_phase]);
}

static double Sigmoid(double E, double K) {
    return 1 / (1 + exp(-K * E));
}

static double SigmoidPrime(double sigmoid) {
    // K is omitted for now but will be added later
    return sigmoid * (1 - sigmoid);
}

static double Cost(TuningData_t* tuningData, double K) {
    double totalError = 0;
    for(int i = 0; i < tuningData->numEntries; i++) {
        TEntry_t entry = tuningData->entryList[i];
        double E = Evaluation(entry);

        double error = entry.result - Sigmoid(E, K);
        totalError += error * error;
    }

    return totalError;
}

static double MSE(TuningData_t* tuningData, double cost) {
    return cost / tuningData->numEntries;
}

static double ComputeK(TuningData_t* tuningData) {
    double start = 0.001;
    double end = 0.01;
    double step = (end - start) / 1000;
    double currentK = start;

    int iter = 0;

    double bestK;
    double lowestCost = 10000000;
    while(currentK <= end) {
        double cost = Cost(tuningData, currentK);
        if(cost <= lowestCost) {
            lowestCost = cost;
            bestK = currentK;
        }
        currentK += step;
        printf("iteration %d\n", iter++);
    }
    
    printf("K = %f gives MSE of %f\n", bestK, MSE(tuningData, lowestCost));

    return bestK;
}

static void UpdateGradPSTComponent(
    TEntry_t entry,
    double coeffs[NUM_PHASES],
    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH]
)
{
    for(Piece_t p = 0; p < NUM_PIECES; p++) {
        Bitboard_t whitePieces = entry.pieceBBs[p] & entry.all[white];
        Bitboard_t blackPieces = entry.pieceBBs[p] & entry.all[black];

        while(whitePieces) {
            Square_t sq = MIRROR(LSB(whitePieces));
            gradient[mg_phase][pst_offset + NUM_SQUARES*p + sq] += coeffs[mg_phase];
            gradient[eg_phase][pst_offset + NUM_SQUARES*p + sq] += coeffs[eg_phase];
            ResetLSB(&whitePieces);
        }
        while(blackPieces) {
            Square_t sq = LSB(blackPieces);
            gradient[mg_phase][pst_offset + NUM_SQUARES*p + sq] -= coeffs[mg_phase];
            gradient[eg_phase][pst_offset + NUM_SQUARES*p + sq] -= coeffs[eg_phase];
            ResetLSB(&blackPieces);
        }
    }
}

static void UpdateGradient(
    TEntry_t entry,
    double K,
    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH]
)
{
    double R = entry.result;
    double sigmoid = Sigmoid(Evaluation(entry), K);
    double sigmoidPrime = SigmoidPrime(sigmoid);

    double coeffs[NUM_PHASES];
    coeffs[mg_phase] = (R - sigmoid) * sigmoidPrime * entry.phaseConstant[mg_phase];
    coeffs[eg_phase] = (R - sigmoid) * sigmoidPrime * entry.phaseConstant[eg_phase];

    UpdateGradPSTComponent(entry, coeffs, gradient);
}

static void UpdateWeights(
    TuningData_t* tuningData,
    double K,
    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH]
)
{
    double coeff = K * ((double)2 / tuningData->numEntries) * LEARN_RATE;

    for(int i = 0; i < VECTOR_LENGTH; i++) {
        weights[mg_phase][i] += coeff * gradient[mg_phase][i];
        weights[eg_phase][i] += coeff * gradient[eg_phase][i];
    }
}

static void CreateOutputFile();

void TuneParameters(const char* filename) {
    TuningData_t tuningData;
    TuningDataInit(&tuningData, filename);

    double K = 0.006634;

    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH];

    double prevCost = Cost(&tuningData, K);
    double startMSE = MSE(&tuningData, prevCost);
    for(int epoch = 0; epoch < MAX_EPOCHS; epoch++) {
        memset(gradient, 0, sizeof(gradient));

        for(int i = 0; i < tuningData.numEntries; i++) {
            TEntry_t entry = tuningData.entryList[i];
            UpdateGradient(entry, K, gradient);
        }

        UpdateWeights(&tuningData, K, gradient);

        double cost = Cost(&tuningData, K);
        double mse = MSE(&tuningData, cost);
        printf("Epoch: %d Cost: %f MSE: %f\n", epoch, cost, mse);
        printf("Cost change since previous epoch: %f\n", cost - prevCost);
        printf("MSE change since epoch 0: %f\n\n", mse - startMSE);

        prevCost = cost;
        
        if(epoch % 10 == 0) {
            CreateOutputFile();
        }
    }

    CreateOutputFile();
    free(tuningData.entryList);
}

// FILE PRINTING BELOW
static void FilePrintPST(const char* tableName, Phase_t phase, Piece_t piece, FILE* fp) {
    fprintf(fp, "#define %s \\\n", tableName);

    for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
        if(sq % 8 == 0) { // first row
            fprintf(fp, "   ");
        }

        fprintf(fp, "%d, ", (int)weights[phase][pst_offset + NUM_SQUARES*piece + sq]);

        if(sq % 8 == 7) { // first row
            fprintf(fp, "\\\n");
        }
    }

    fprintf(fp, "\n");
}

static void AddPieceValComment(FILE* fp) {
    const char* names[NUM_PIECES - 1] = { "Knight", "Bishop", "Rook", "Queen", "Pawn" };

    fprintf(fp, "/*\n");
    fprintf(fp, "Average PST values for MG, EG:\n");

    for(Piece_t p = 0; p < NUM_PIECES - 1; p++) {
        double mgSum = 0;
        double egSum = 0;
        for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
            mgSum += weights[mg_phase][pst_offset + NUM_SQUARES*p + sq];
            egSum += weights[eg_phase][pst_offset + NUM_SQUARES*p + sq];
        }

        int mgValue = mgSum / NUM_SQUARES;
        int egValue = egSum / NUM_SQUARES;

        fprintf(fp, "%s Values: %d %d\n", names[p], mgValue, egValue);
    }

    fprintf(fp, "*/\n\n");
}

static void CreateOutputFile() {
    FILE* fp = fopen("tuning_output.txt", "w");

    AddPieceValComment(fp);

    FilePrintPST("PAWN_MG_PST", mg_phase, pawn, fp);
    FilePrintPST("PAWN_EG_PST", eg_phase, pawn, fp);

    FilePrintPST("KNIGHT_MG_PST", mg_phase, knight, fp);
    FilePrintPST("KNIGHT_EG_PST", eg_phase, knight, fp);

    FilePrintPST("BISHOP_MG_PST", mg_phase, bishop, fp);
    FilePrintPST("BISHOP_EG_PST", eg_phase, bishop, fp);

    FilePrintPST("ROOK_MG_PST", mg_phase, rook, fp);
    FilePrintPST("ROOK_EG_PST", eg_phase, rook, fp);

    FilePrintPST("QUEEN_MG_PST", mg_phase, queen, fp);
    FilePrintPST("QUEEN_EG_PST", eg_phase, queen, fp);
    
    FilePrintPST("KING_MG_PST", mg_phase, king, fp);
    FilePrintPST("KING_EG_PST", eg_phase, king, fp);

    fclose(fp);
}
