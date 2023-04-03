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
#include "eval_constants_old.h"
#include "util_macros.h"

#include "chess_search.h"
#include "engine_types.h"

enum {
    PST_FEATURE_COUNT = 8 * 8 * 2 * NUM_PIECES * NUM_SQUARES,
    BISHOP_PAIR_FEATURE_COUNT = 1,

    pst_offset = 0,
    bishop_pair_offset = PST_FEATURE_COUNT,

    VECTOR_LENGTH,
};

enum {
    LINE_BUFFER = 500,
    MAX_EPOCHS = 10000,
    ENTRY_MAX = 100000,
};

#define LEARN_RATE 5000

typedef double Gradient_t;
typedef double Weight_t;
typedef double Velocity_t;
typedef double Momentum_t;

typedef uint8_t Bucket_t;

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

static int FlatPSTIndex(Bucket_t w, Bucket_t b, Color_t c, Piece_t p, Square_t sq) {
    return (8*2*6*64)*w + (2*6*64)*b + (6*64)*c + (64)*p + sq;
}

static void InitWeights() {
    Weight_t tempPST[NUM_PHASES][384] = {
        { KNIGHT_MG_PST BISHOP_MG_PST ROOK_MG_PST QUEEN_MG_PST PAWN_MG_PST KING_MG_PST },
        { KNIGHT_EG_PST BISHOP_EG_PST ROOK_EG_PST QUEEN_EG_PST PAWN_EG_PST KING_EG_PST },
    };
    Weight_t tempPair[] = { BISHOP_PAIR_BONUS };

    for(Bucket_t w = 0; w < 8; w++) {
        for(Bucket_t b = 0; b < 8; b++) {
            for(int c = 0; c < 2; c++) {
                for(int p = 0; p < NUM_PIECES; p++) {
                    for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
                        for(Phase_t phase = 0; phase < 2; phase++) {
                            Square_t pstSq = (c == black) ? sq : MIRROR(sq);
                            weights[phase][FlatPSTIndex(w, b, c, p, pstSq)] = tempPST[phase][NUM_SQUARES*p + sq];
                        }
                    }
                }
            }
        }
    }

    weights[mg_phase][bishop_pair_offset] = tempPair[mg_phase];
    weights[eg_phase][bishop_pair_offset] = tempPair[eg_phase];
}

static void FillPSTFeatures(
    Square_t wKing,
    Square_t bKing,
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
            Square_t sq = LSB(whitePieces);
            allValues[FlatPSTIndex(wKing/8, bKing/8, white, piece, sq)]++;
            ResetLSB(&whitePieces);
        }
        while(blackPieces) {
            Square_t sq = LSB(blackPieces);
            allValues[FlatPSTIndex(wKing/8, bKing/8, black, piece, sq)]--;
            ResetLSB(&blackPieces);
        }
    }
}

static void FillBonuses(
    int16_t allValues[VECTOR_LENGTH],
    BoardInfo_t* boardInfo
)
{
    allValues[bishop_pair_offset] += 
        (int)(PopCount(boardInfo->bishops[white]) >= 2) - 
        (int)(PopCount(boardInfo->bishops[black]) >= 2);
}

void FillTEntry(TEntry_t* tEntry, BoardInfo_t* boardInfo) {
    int16_t allValues[VECTOR_LENGTH] = {0};

    FillPSTFeatures(KingSquare(boardInfo, white), KingSquare(boardInfo, black), allValues, boardInfo);
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

static void TuningDataInit(TuningData_t* tuningData, FILE* fp, int numEntries) {
    tuningData->numEntries = numEntries;
    tuningData->entryList = malloc(tuningData->numEntries * sizeof(TEntry_t));

    char buffer[LINE_BUFFER];
    for(int i = 0; i < tuningData->numEntries; i++) {
        fgets(buffer, LINE_BUFFER, fp);

        int resultIndex = 0;
        while(buffer[resultIndex + 1] != '.') {
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

        if(buffer[resultIndex] == '0') {
            if(buffer[resultIndex + 2] == '0') {
                tuningData->entryList[i].result = 0;
            } else {
                tuningData->entryList[i].result = 0.5;
            }
        } else {
            assert(buffer[resultIndex] == '1');
            tuningData->entryList[i].result = 1;
        }
    }

    printf("%d Entries Loaded\n", numEntries);
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

static double MSE(uint64_t numEntries, double cost) {
    return cost / numEntries;
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
    uint64_t numEntries,
    double K,
    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH]
)
{
    const double beta1 = .9;
    const double beta2 = .999;
    const double epsilon = 1e-8;

    for(int i = 0; i < VECTOR_LENGTH; i++) {
        for(Phase_t phase = 0; phase < NUM_PHASES; phase++) {
            Gradient_t grad = -K * gradient[phase][i] / numEntries;
            momentum[phase][i] = beta1 * momentum[phase][i] + (1 - beta1) * grad;
            velocity[phase][i] = beta2 * velocity[phase][i] + (1 - beta2) * pow(grad, 2);

            weights[phase][i] -= momentum[phase][i] / (epsilon + sqrt(velocity[phase][i]));
        }
    }
}

static void CreateOutputFile();

void TuneParameters(const char* filename) {
    FILE* fp = fopen(filename, "r");

    InitWeights();
    printf("Finding file length\n");
    uint64_t totalEntries = EntriesInFile(fp);
    printf("File length: %lld\n", (long long)totalEntries);

    TuningData_t tuningData;

    double K = 0.006634;

    Gradient_t gradient[NUM_PHASES][VECTOR_LENGTH];

    double mse;
    double prevMSE = 10000000000;
    for(int epoch = 0; epoch < MAX_EPOCHS; epoch++) {
        uint64_t remainingEntries = totalEntries;

        while(remainingEntries) {
            memset(gradient, 0, sizeof(gradient));

            int using = MIN(ENTRY_MAX, remainingEntries);
            remainingEntries -= using;
            TuningDataInit(&tuningData, fp, using);

            for(int i = 0; i < using; i++) {
                TEntry_t entry = tuningData.entryList[i];
                UpdateGradient(entry, K, gradient);
            }
            
            UpdateWeights(using, K, gradient);

            mse = Cost(&tuningData, K) / using;
            if(prevMSE - mse < 0) {
                printf("CONVERGED!\n");
                remainingEntries = 0;
            } else {
                prevMSE = mse;
            }

            TuningDataTeardown(&tuningData);
        }

        printf("Epoch: %d\n", epoch);
        printf("MSE change since previous: %f\n\n", mse - prevMSE);

        if(prevMSE - mse <= 0) {
            printf("CONVERGED!\n");
            break;
        }

        prevMSE = mse;
        CreateOutputFile();
    }

    CreateOutputFile();

    fclose(fp);
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
static void FilePrintPST(FILE* fp) {
    fprintf(fp, "#define SUPER_PST_MG \\\n");

    for(int i = 0; i < PST_FEATURE_COUNT; i++) {
        fprintf(fp, "%d, ", (int)weights[mg_phase][i]);
    }

    fprintf(fp, "\n\n#define SUPER_PST_EG \\\n");

    for(int i = 0; i < PST_FEATURE_COUNT; i++) {
        fprintf(fp, "%d, ", (int)weights[eg_phase][i]);
    }

    fprintf(fp, "\n");
}

static void PrintBonuses(FILE* fp) {
    fprintf(fp, "#define BISHOP_PAIR_BONUS \\\n   %d, %d\n\n",
        (int)weights[mg_phase][bishop_pair_offset],
        (int)weights[eg_phase][bishop_pair_offset]
    );
}

static void CreateOutputFile() {
    FILE* fp = fopen("tuning_output.txt", "w");

    PrintBonuses(fp);

    FilePrintPST(fp);

    fclose(fp);
}
