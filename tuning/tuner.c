#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
#include "chess_search.h"
#include "engine_types.h"
#include "eval_helpers.h"

enum {
    PST_FEATURE_COUNT = NUM_PIECES * NUM_SQUARES,
    BISHOP_PAIR_FEATURE_COUNT = 1,
    PASSED_PAWN_FEATURE_COUNT = NUM_SQUARES,
    BLOCKED_PASSER_FEATURE_COUNT = 8,

    pst_offset = 0,
    bishop_pair_offset = pst_offset + PST_FEATURE_COUNT,
    passed_pawn_offset = bishop_pair_offset + BISHOP_PAIR_FEATURE_COUNT,
    blocked_passer_offset = passed_pawn_offset + PASSED_PAWN_FEATURE_COUNT,

    VECTOR_LENGTH = blocked_passer_offset + BLOCKED_PASSER_FEATURE_COUNT,
};

enum {
    LINE_BUFFER = 500,
    MAX_EPOCHS = 10000,
};

typedef double Gradient_t;
typedef double Weight_t;
typedef double Velocity_t;
typedef double Momentum_t;

Weight_t weights[NUM_PHASES][VECTOR_LENGTH] = {0};
Velocity_t velocity[NUM_PHASES][VECTOR_LENGTH] = {0};
Momentum_t momentum[NUM_PHASES][VECTOR_LENGTH] = {0};

typedef struct {
    int16_t value;
    int32_t index;
} Feature_t;

typedef struct {
    Feature_t* features;
    uint16_t numFeatures;

    double phaseConstant[NUM_PHASES];
    double result;
} TEntry_t;

typedef struct {
    TEntry_t* entryList;
    int numEntries;
} TuningData_t;

static void FillPSTFeatures(
    int16_t allValues[VECTOR_LENGTH],
    BoardInfo_t* boardInfo
)
{
    Bitboard_t* pieces[NUM_PIECES] = {
        boardInfo->knights,
        boardInfo->bishops,
        boardInfo->rooks,
        boardInfo->queens,
        boardInfo->pawns,
        boardInfo->kings,
    };

    for(Piece_t piece = 0; piece < NUM_PIECES; piece++) {
        Bitboard_t whitePieces = pieces[piece][white];
        Bitboard_t blackPieces = pieces[piece][black];

        while(whitePieces) {
            Square_t sq = MIRROR(LSB(whitePieces));
            allValues[pst_offset + NUM_SQUARES*piece + sq]++;
            ResetLSB(&whitePieces);
        }
        while(blackPieces) {
            Square_t sq = LSB(blackPieces);
            allValues[pst_offset + NUM_SQUARES*piece + sq]--;
            ResetLSB(&blackPieces);
        }
    }
}

static void FillBonuses(
    int16_t allValues[VECTOR_LENGTH],
    BoardInfo_t* boardInfo
)
{
    // BISHOP PAIR
    allValues[bishop_pair_offset] += 
        (int)(PopCount(boardInfo->bishops[white]) >= 2) - 
        (int)(PopCount(boardInfo->bishops[black]) >= 2);

    // PASSED PAWN
    const Bitboard_t wFrontSpan = WhiteForwardFill(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackForwardFill(boardInfo->pawns[black]);

    const Bitboard_t wPawnBlocks = wFrontSpan | EastOne(wFrontSpan) | WestOne(wFrontSpan);
    const Bitboard_t bPawnBlocks = bFrontSpan | EastOne(bFrontSpan) | WestOne(bFrontSpan);

    Bitboard_t wPassers = boardInfo->pawns[white] & ~bPawnBlocks;
    Bitboard_t bPassers = boardInfo->pawns[black] & ~wPawnBlocks;

    Bitboard_t piecesBlockingWhite = NortOne(wPassers) & boardInfo->allPieces[black];
    Bitboard_t piecesBlockingBlack = SoutOne(bPassers) & boardInfo->allPieces[white];

    while(wPassers) {
        Square_t sq = MIRROR(LSB(wPassers));
        allValues[passed_pawn_offset + sq]++;
        ResetLSB(&wPassers);
    }
    while(bPassers) {
        Square_t sq = LSB(bPassers);
        allValues[passed_pawn_offset + sq]--;
        ResetLSB(&bPassers);
    }

    // BLOCKED PASSER
    while(piecesBlockingWhite) {
        Rank_t rank = (MIRROR(LSB(piecesBlockingWhite))) / 8;
        allValues[blocked_passer_offset + rank]++;
        ResetLSB(&piecesBlockingWhite);
    }
    while(piecesBlockingBlack) {
        Rank_t rank = LSB(piecesBlockingBlack) / 8;
        allValues[blocked_passer_offset + rank]--;
        ResetLSB(&piecesBlockingBlack);
    }
}

void FillTEntry(TEntry_t* tEntry, BoardInfo_t* boardInfo) {
    int16_t allValues[VECTOR_LENGTH] = {0};

    FillPSTFeatures(allValues, boardInfo);
    FillBonuses(allValues, boardInfo);

    tEntry->numFeatures = 0;
    for(uint16_t i = 0; i < VECTOR_LENGTH; i++) {
        if(allValues[i] != 0) {
            tEntry->numFeatures++;
        }
    }

    tEntry->features = malloc(sizeof(Feature_t) * tEntry->numFeatures);
    assert(tEntry->features != NULL);
    
    uint16_t featureIndex = 0;
    for(uint16_t i = 0; i < VECTOR_LENGTH; i++) {
        if(allValues[i] != 0) {
            tEntry->features[featureIndex].value = allValues[i];
            tEntry->features[featureIndex].index = i;
            featureIndex++;
        }
    }

    // phase calcuation
    Phase_t midgame_phase = 
        PopCount(boardInfo->knights[white] | boardInfo->knights[black])*KNIGHT_PHASE_VALUE +
        PopCount(boardInfo->bishops[white] | boardInfo->bishops[black])*BISHOP_PHASE_VALUE +
        PopCount(boardInfo->rooks[white] | boardInfo->rooks[black])*ROOK_PHASE_VALUE +
        PopCount(boardInfo->queens[white] | boardInfo->queens[black])*QUEEN_PHASE_VALUE;

    midgame_phase = MIN(midgame_phase, PHASE_MAX);

    tEntry->phaseConstant[mg_phase] = (double)midgame_phase / PHASE_MAX;
    tEntry->phaseConstant[eg_phase] = 1 - tEntry->phaseConstant[mg_phase];
}

static int EntriesInFile(FILE* fp) {
    uint64_t lines = 0;

    char buffer[LINE_BUFFER];
    while(fgets(buffer, LINE_BUFFER, fp)) {
        if(strlen(buffer) <= 2) {
            break;
        }
        lines++;
    }

    rewind(fp);

    return lines;
}

static void TuningDataInit(TuningData_t* tuningData, const char* filename) {
    FILE* fp = fopen(filename, "r");
    
    tuningData->numEntries = EntriesInFile(fp);
    tuningData->entryList = malloc(tuningData->numEntries * sizeof(TEntry_t));

    char buffer[LINE_BUFFER];
    int percentComplete = -1;
    for(int i = 0; i < tuningData->numEntries; i++) {
        fgets(buffer, LINE_BUFFER, fp);

        int resultIndex = 0;
        while(buffer[resultIndex] != '"') {
            resultIndex++; 
        }

        char fenBuffer[FEN_BUFFER_SIZE];
        memset(fenBuffer, '\0', FEN_BUFFER_SIZE);
        memcpy(fenBuffer, buffer, resultIndex * sizeof(char));

        BoardInfo_t boardInfo;
        GameStack_t gameStack;
        ZobristStack_t zobristStack;
        InterpretFEN(fenBuffer, &boardInfo, &gameStack, &zobristStack);

        FillTEntry(&tuningData->entryList[i], &boardInfo);

        resultIndex++; 
        if(buffer[resultIndex] == '1') {
            if(buffer[resultIndex + 1] == '-') {
                tuningData->entryList[i].result = 1;
            } else {
                tuningData->entryList[i].result = 0.5;
            }
        } else {
            assert(buffer[resultIndex] == '0');
            tuningData->entryList[i].result = 0;
        }

        if(percentComplete < (100*i / tuningData->numEntries)) {
            percentComplete = (100*i / tuningData->numEntries);
            printf("Data %d%% loaded\n", percentComplete);
        }
    }

    fclose(fp);
}

static void TuningDataTeardown(TuningData_t* tuningData) {
    for(int i = 0; i < tuningData->numEntries; i++) {
        TEntry_t entry = tuningData->entryList[i];
        free(entry.features);
    }
    free(tuningData->entryList);
}

static double Evaluation(TEntry_t entry) {
    double mgScore = 0;
    double egScore = 0;

    for(int i = 0; i < entry.numFeatures; i++) {
        Feature_t feature = entry.features[i];

        mgScore += feature.value * weights[mg_phase][feature.index];
        egScore += feature.value * weights[eg_phase][feature.index];
    }

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

    for(int i = 0; i < entry.numFeatures; i++) {
        Feature_t feature = entry.features[i];

        gradient[mg_phase][feature.index] += coeffs[mg_phase] * feature.value;
        gradient[eg_phase][feature.index] += coeffs[eg_phase] * feature.value;
    }
}

static void UpdateWeights(
    TuningData_t* tuningData,
    double K,
    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH]
)
{
    const double beta1 = .9;
    const double beta2 = .999;
    const double epsilon = 1e-8;

    for(int i = 0; i < VECTOR_LENGTH; i++) {
        for(Phase_t phase = 0; phase < NUM_PHASES; phase++) {
            Gradient_t grad = -K * gradient[phase][i] / tuningData->numEntries;
            momentum[phase][i] = beta1 * momentum[phase][i] + (1 - beta1) * grad;
            velocity[phase][i] = beta2 * velocity[phase][i] + (1 - beta2) * pow(grad, 2);

            weights[phase][i] -= momentum[phase][i] / (epsilon + sqrt(velocity[phase][i]));
        }
    }
}

static void CreateOutputFile();

void TuneParameters(const char* filename) {
    TuningData_t tuningData;
    TuningDataInit(&tuningData, filename);

    double K = 0.006634;

    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH];

    double prevCost = Cost(&tuningData, K);
    double prevMSE = MSE(&tuningData, prevCost);
    for(int epoch = 0; epoch < MAX_EPOCHS; epoch++) {
        memset(gradient, 0, sizeof(gradient));

        for(int i = 0; i < tuningData.numEntries; i++) {
            TEntry_t entry = tuningData.entryList[i];
            UpdateGradient(entry, K, gradient);
        }

        UpdateWeights(&tuningData, K, gradient);


        if(epoch % 25 == 0) {
            double cost = Cost(&tuningData, K);
            double mse = MSE(&tuningData, cost);
            printf("Epoch: %d Cost: %f MSE: %.7f\n", epoch, cost, mse);
            printf("Cost change since previous: %f\n", cost - prevCost);
            printf("MSE change since previous: %f\n\n", mse - prevMSE);

            if(prevMSE - mse < 1e-7) {
                printf("CONVERGED!\n");
                break;
            }

            prevCost = cost;
            prevMSE = mse;
            CreateOutputFile();
        }
    }

    CreateOutputFile();
    TuningDataTeardown(&tuningData);
}

void FilterNonQuiets(const char* filename) {
    FILE* rFP = fopen(filename, "r");
    FILE* wFP = fopen("resolved.txt", "w");

    uint64_t writes = 0;
    uint64_t reads = 0;

    char buffer[LINE_BUFFER];
    while(fgets(buffer, LINE_BUFFER, rFP)) {
        reads++;

        BoardInfo_t boardInfo;
        GameStack_t gameStack;
        ZobristStack_t zobristStack;
        InterpretFEN(buffer, &boardInfo, &gameStack, &zobristStack);

        EvalScore_t staticEval = ScoreOfPosition(&boardInfo);
        EvalScore_t qsearchEval = SimpleQsearch(
            &boardInfo,
            &gameStack,
            -INF,
            INF
        );

        if(staticEval == qsearchEval) {
            fprintf(wFP, "%s", buffer);
            writes++;
        }

        if(reads % 100000 == 0) {
            printf("%lld reads\n%lld saved\n\n", (long long)reads, (long long) writes);
        }
    }

    printf("%lld reads\n%lld saved\n\n", (long long)reads, (long long) writes);

    fclose(rFP);
    fclose(wFP);
}

// FILE PRINTING BELOW
static void FilePrintPST(const char* tableName, Phase_t phase, Piece_t piece, FILE* fp, int offset) {
    fprintf(fp, "#define %s \\\n", tableName);

    for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
        if(sq % 8 == 0) { // first row entry
            fprintf(fp, "   ");
        }

        fprintf(fp, "%d, ", (int)weights[phase][offset + NUM_SQUARES*piece + sq]);

        if(sq % 8 == 7) { // last row entry
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
        int squareCount = (p == pawn) ? NUM_SQUARES - 16 : NUM_SQUARES;

        double mgSum = 0;
        double egSum = 0;
        for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
            mgSum += weights[mg_phase][pst_offset + NUM_SQUARES*p + sq];
            egSum += weights[eg_phase][pst_offset + NUM_SQUARES*p + sq];
        }

        int mgValue = mgSum / squareCount;
        int egValue = egSum / squareCount;

        fprintf(fp, "%s Values: %d %d\n", names[p], mgValue, egValue);
    }

    fprintf(fp, "*/\n\n");
}

static void PrintBonuses(FILE* fp) {
    fprintf(fp, "#define BISHOP_PAIR_BONUS \\\n   %d, %d\n\n",
        (int)weights[mg_phase][bishop_pair_offset],
        (int)weights[eg_phase][bishop_pair_offset]
    );

    FilePrintPST("PASSED_PAWN_MG_PST", mg_phase, 0, fp, passed_pawn_offset);
    FilePrintPST("PASSED_PAWN_EG_PST", eg_phase, 0, fp, passed_pawn_offset);

    fprintf(fp, "#define BLOCKED_PASSERS_MG \\\n   ");
    for(int i = 0; i < 8; i++) { 
        fprintf(fp, "%d, ", (int)weights[mg_phase][blocked_passer_offset + i]);
    }

    fprintf(fp, "#define BLOCKED_PASSERS_EG \\\n   ");
    for(int i = 0; i < 8; i++) { 
        fprintf(fp, "%d, ", (int)weights[eg_phase][blocked_passer_offset + i]);
    }
}

static void CreateOutputFile() {
    FILE* fp = fopen("tuning_output.txt", "w");

    PrintBonuses(fp);

    AddPieceValComment(fp);

    FilePrintPST("KNIGHT_MG_PST", mg_phase, knight, fp, pst_offset);
    FilePrintPST("KNIGHT_EG_PST", eg_phase, knight, fp, pst_offset);

    FilePrintPST("BISHOP_MG_PST", mg_phase, bishop, fp, pst_offset);
    FilePrintPST("BISHOP_EG_PST", eg_phase, bishop, fp, pst_offset);

    FilePrintPST("ROOK_MG_PST", mg_phase, rook, fp, pst_offset);
    FilePrintPST("ROOK_EG_PST", eg_phase, rook, fp, pst_offset);

    FilePrintPST("QUEEN_MG_PST", mg_phase, queen, fp, pst_offset);
    FilePrintPST("QUEEN_EG_PST", eg_phase, queen, fp, pst_offset);

    FilePrintPST("PAWN_MG_PST", mg_phase, pawn, fp, pst_offset);
    FilePrintPST("PAWN_EG_PST", eg_phase, pawn, fp, pst_offset);
    
    FilePrintPST("KING_MG_PST", mg_phase, king, fp, pst_offset);
    FilePrintPST("KING_EG_PST", eg_phase, king, fp, pst_offset);

    fclose(fp);
}
