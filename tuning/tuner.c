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
#include "eval_constants_tools.h"
#include "util_macros.h"
#include "chess_search.h"
#include "engine_types.h"
#include "eval_helpers.h"
#include "attack_eval.h"
#include "lookup.h"

enum {
    PST_FEATURE_COUNT = NUM_PST_BUCKETS * NUM_PIECES * NUM_SQUARES,
    BISHOP_PAIR_FEATURE_COUNT = 1,
    PASSED_PAWN_FEATURE_COUNT = NUM_PST_BUCKETS * NUM_SQUARES,
    BLOCKED_PASSER_FEATURE_COUNT = NUM_RANKS,
    OPEN_FILE_FEATURE_COUNT = NUM_FILES,
    ISOLATED_FEATURE_COUNT = NUM_FILES,
    KNIGHT_MOBILITY_FEATURE_COUNT = KNIGHT_MOBILITY_OPTIONS,
    BISHOP_MOBILITY_FEATURE_COUNT = BISHOP_MOBILITY_OPTIONS,
    ROOK_MOBILITY_FEATURE_COUNT = ROOK_MOBILITY_OPTIONS,
    QUEEN_MOBILITY_FEATURE_COUNT = QUEEN_MOBILITY_OPTIONS,
    THREAT_FEATURE_COUNT = 1,

    pst_offset = 0,
    bishop_pair_offset = pst_offset + PST_FEATURE_COUNT,
    passed_pawn_offset = bishop_pair_offset + BISHOP_PAIR_FEATURE_COUNT,
    blocked_passer_offset = passed_pawn_offset + PASSED_PAWN_FEATURE_COUNT,
    open_rook_offset = blocked_passer_offset + BLOCKED_PASSER_FEATURE_COUNT,
    semi_open_rook_offset = open_rook_offset + OPEN_FILE_FEATURE_COUNT,
    open_queen_offset = semi_open_rook_offset + OPEN_FILE_FEATURE_COUNT,
    semi_open_queen_offset = open_queen_offset + OPEN_FILE_FEATURE_COUNT,
    open_king_offset = semi_open_queen_offset + OPEN_FILE_FEATURE_COUNT,
    semi_open_king_offset = open_king_offset + OPEN_FILE_FEATURE_COUNT,

    isolated_pawns_offset = semi_open_king_offset + OPEN_FILE_FEATURE_COUNT,

    knight_mobility_offset = isolated_pawns_offset + ISOLATED_FEATURE_COUNT,
    bishop_mobility_offset = knight_mobility_offset + KNIGHT_MOBILITY_FEATURE_COUNT,
    rook_mobility_offset = bishop_mobility_offset + BISHOP_MOBILITY_FEATURE_COUNT,
    queen_mobility_offset = rook_mobility_offset + ROOK_MOBILITY_FEATURE_COUNT,

    pawn_threat_on_knight = queen_mobility_offset + QUEEN_MOBILITY_FEATURE_COUNT,
    pawn_threat_on_bishop = pawn_threat_on_knight + THREAT_FEATURE_COUNT,
    pawn_threat_on_rook = pawn_threat_on_bishop + THREAT_FEATURE_COUNT,
    pawn_threat_on_queen = pawn_threat_on_rook + THREAT_FEATURE_COUNT,

    knight_threat_on_bishop = pawn_threat_on_queen + THREAT_FEATURE_COUNT,
    knight_threat_on_rook = knight_threat_on_bishop + THREAT_FEATURE_COUNT,
    knight_threat_on_queen = knight_threat_on_rook + THREAT_FEATURE_COUNT,

    bishop_threat_on_knight = knight_threat_on_queen + THREAT_FEATURE_COUNT,
    bishop_threat_on_rook = bishop_threat_on_knight + THREAT_FEATURE_COUNT,
    bishop_threat_on_queen = bishop_threat_on_rook + THREAT_FEATURE_COUNT,

    rook_threat_on_queen = bishop_threat_on_queen + THREAT_FEATURE_COUNT,

    VECTOR_LENGTH = rook_threat_on_queen + THREAT_FEATURE_COUNT,
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

static int PSTSetOffset(Bucket_t bucket, Piece_t piece, Square_t sq, int featureOffset) {
    return featureOffset + ((NUM_PIECES*NUM_SQUARES)*bucket + NUM_SQUARES*piece + sq);
}

static int PSTSingleOffset(Bucket_t bucket, Square_t sq, int featureOffset) {
    return featureOffset + (NUM_SQUARES)*bucket + sq;
}

static void TunerSerializeBySquare(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    const Bucket_t wBucket,
    const Bucket_t bBucket,
    int feature_offset,
    int16_t allValues[VECTOR_LENGTH]
)
{
    while(whitePieces) {
        Square_t sq = MIRROR(LSB(whitePieces));
        allValues[PSTSingleOffset(wBucket, sq, feature_offset)]++;
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        allValues[PSTSingleOffset(bBucket, sq, feature_offset)]--;
        ResetLSB(&blackPieces);
    }
}

static void TunerSerializeByFile(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    int feature_offset,
    int16_t allValues[VECTOR_LENGTH]
)
{
    while(whitePieces) {
        File_t file = LSB(whitePieces) % 8;
        allValues[feature_offset + file]++;
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        File_t file = LSB(blackPieces) % 8;
        allValues[feature_offset + file]--;
        ResetLSB(&blackPieces);
    }
}

static void TunerSerializeByRank(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    int feature_offset,
    int16_t allValues[VECTOR_LENGTH]
)
{
    while(whitePieces) {
        Rank_t rank = (MIRROR(LSB(whitePieces))) / 8;
        allValues[feature_offset + rank]++;
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Rank_t rank = LSB(blackPieces) / 8;
        allValues[feature_offset + rank]--;
        ResetLSB(&blackPieces);
    }
}

static void TunerComputeKnights(
    BoardInfo_t* boardInfo,
    Bitboard_t knights,
    Bitboard_t availible,
    int16_t allValues[VECTOR_LENGTH],
    int multiplier
)
{
    Color_t color = multiplier == 1 ? white : black;
    const Bitboard_t enemyBishops = boardInfo->bishops[!color];
    const Bitboard_t enemyRooks = boardInfo->rooks[!color];
    const Bitboard_t enemyQueens = boardInfo->queens[!color];
    while(knights) {
        Square_t sq = LSB(knights);
        Bitboard_t moves = GetKnightAttackSet(sq) & availible;
        allValues[knight_mobility_offset + PopCount(moves)] += multiplier;

        allValues[knight_threat_on_bishop] += PopCount(moves & enemyBishops) * multiplier;
        allValues[knight_threat_on_rook] += PopCount(moves & enemyRooks) * multiplier;
        allValues[knight_threat_on_queen] += PopCount(moves & enemyQueens) * multiplier;

        ResetLSB(&knights);
    }
}

static void TunerComputeBishops(
    BoardInfo_t* boardInfo,
    Bitboard_t bishops,
    Bitboard_t availible,
    Bitboard_t d12Empty,
    int16_t allValues[VECTOR_LENGTH],
    int multiplier
)
{
    Color_t color = multiplier == 1 ? white : black;
    const Bitboard_t enemyKnights = boardInfo->knights[!color];
    const Bitboard_t enemyRooks = boardInfo->rooks[!color];
    const Bitboard_t enemyQueens = boardInfo->queens[!color];
    while(bishops) {
        Square_t sq = LSB(bishops);
        Bitboard_t moves = GetBishopAttackSet(sq, d12Empty) & availible;
        allValues[bishop_mobility_offset + PopCount(moves)] += multiplier;

        allValues[bishop_threat_on_knight] += PopCount(moves & enemyKnights) * multiplier;
        allValues[bishop_threat_on_rook] += PopCount(moves & enemyRooks) * multiplier;
        allValues[bishop_threat_on_queen] += PopCount(moves & enemyQueens) * multiplier;

        ResetLSB(&bishops);
    }
}

static void TunerComputeRooks(
    BoardInfo_t* boardInfo,
    Bitboard_t rooks, 
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    int16_t allValues[VECTOR_LENGTH],
    int multiplier
)
{
    Color_t color = multiplier == 1 ? white : black;
    const Bitboard_t enemyQueens = boardInfo->queens[!color];
    while(rooks) {
        Square_t sq = LSB(rooks);
        Bitboard_t moves = GetRookAttackSet(sq, hvEmpty) & availible;
        allValues[rook_mobility_offset + PopCount(moves)] += multiplier;

        allValues[rook_threat_on_queen] += PopCount(moves & enemyQueens) * multiplier;

        ResetLSB(&rooks);
    }  
}

static void TunerComputeQueens(
    Bitboard_t queens,
    Bitboard_t availible,
    Bitboard_t hvEmpty,
    Bitboard_t d12Empty,
    int16_t allValues[VECTOR_LENGTH],
    int multiplier
)
{
    while(queens) {
        Square_t sq = LSB(queens);
        Bitboard_t d12Moves = GetBishopAttackSet(sq, d12Empty) & availible;
        Bitboard_t hvMoves = GetRookAttackSet(sq, hvEmpty) & availible;
        allValues[queen_mobility_offset + PopCount(d12Moves) + PopCount(hvMoves)] += multiplier;
        ResetLSB(&queens);
    } 
}

static void PawnThreats(BoardInfo_t* boardInfo, const Bitboard_t wPawnAttacks, const Bitboard_t bPawnAttacks, int16_t allValues[VECTOR_LENGTH]) {
    int knightThreats = PopCount(wPawnAttacks & boardInfo->knights[black]) - PopCount(bPawnAttacks & boardInfo->knights[white]);
    int bishopThreats = PopCount(wPawnAttacks & boardInfo->bishops[black]) - PopCount(bPawnAttacks & boardInfo->bishops[white]);
    int rookThreats = PopCount(wPawnAttacks & boardInfo->rooks[black]) - PopCount(bPawnAttacks & boardInfo->rooks[white]);
    int queenThreats = PopCount(wPawnAttacks & boardInfo->queens[black]) - PopCount(bPawnAttacks & boardInfo->queens[white]);

    allValues[pawn_threat_on_knight] += knightThreats;
    allValues[pawn_threat_on_bishop] += bishopThreats;
    allValues[pawn_threat_on_rook] += rookThreats;
    allValues[pawn_threat_on_queen] += queenThreats;
}

void FillMobility(BoardInfo_t* boardInfo, int16_t allValues[VECTOR_LENGTH]) {
    const Bitboard_t wPawnAttacks = 
        NoEaOne(boardInfo->pawns[white]) | 
        NoWeOne(boardInfo->pawns[white]);
    const Bitboard_t bPawnAttacks = 
        SoEaOne(boardInfo->pawns[black]) |
        SoWeOne(boardInfo->pawns[black]);

    const Bitboard_t wAvailible = ~bPawnAttacks & (boardInfo->allPieces[black] | boardInfo->empty);
    const Bitboard_t bAvailible = ~wPawnAttacks & (boardInfo->allPieces[white] | boardInfo->empty);

    const Bitboard_t whiteHvEmpty = boardInfo->empty | boardInfo->rooks[white] | boardInfo->queens[white];
    const Bitboard_t whiteD12Empty = boardInfo->empty | boardInfo->bishops[white] | boardInfo->queens[white];

    const Bitboard_t blackHvEmpty = boardInfo->empty | boardInfo->rooks[black] | boardInfo->queens[black];
    const Bitboard_t blackD12Empty = boardInfo->empty | boardInfo->bishops[black] | boardInfo->queens[black];

    TunerComputeKnights(boardInfo, boardInfo->knights[white], wAvailible, allValues, 1);
    TunerComputeBishops(boardInfo, boardInfo->bishops[white], wAvailible, whiteD12Empty, allValues, 1);
    TunerComputeRooks(boardInfo, boardInfo->rooks[white], wAvailible, whiteHvEmpty, allValues, 1);
    TunerComputeQueens(boardInfo->queens[white], wAvailible, whiteHvEmpty, whiteD12Empty, allValues, 1);

    TunerComputeKnights(boardInfo, boardInfo->knights[black], bAvailible, allValues, -1);
    TunerComputeBishops(boardInfo, boardInfo->bishops[black], bAvailible, blackD12Empty, allValues, -1);
    TunerComputeRooks(boardInfo, boardInfo->rooks[black], bAvailible, blackHvEmpty, allValues, -1);
    TunerComputeQueens(boardInfo->queens[black], bAvailible, blackHvEmpty, blackD12Empty, allValues, -1);

    PawnThreats(boardInfo, wPawnAttacks, bPawnAttacks, allValues);
}

static void FillPSTFeatures(
    int16_t allValues[VECTOR_LENGTH],
    const Bucket_t whiteBucket,
    const Bucket_t blackBucket,
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
            allValues[PSTSetOffset(whiteBucket, piece, sq, pst_offset)]++;
            ResetLSB(&whitePieces);
        }
        while(blackPieces) {
            Square_t sq = LSB(blackPieces);
            allValues[PSTSetOffset(blackBucket, piece, sq, pst_offset)]--;
            ResetLSB(&blackPieces);
        }
    }
}

static void FillBonuses(
    int16_t allValues[VECTOR_LENGTH],
    const Bucket_t whiteBucket,
    const Bucket_t blackBucket,
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

    TunerSerializeBySquare(wPassers, bPassers, whiteBucket, blackBucket, passed_pawn_offset, allValues);

    // BLOCKED PASSER
    TunerSerializeByRank(piecesBlockingWhite, piecesBlockingBlack, blocked_passer_offset, allValues);

    // OPEN AND SEMI OPEN FILES
    const Bitboard_t whitePawnFileSpans = FileFill(boardInfo->pawns[white]);
    const Bitboard_t blackPawnFileSpans = FileFill(boardInfo->pawns[black]);

    const Bitboard_t openFiles = ~(whitePawnFileSpans | blackPawnFileSpans);
    const Bitboard_t whitePawnOnlyFiles = whitePawnFileSpans & ~blackPawnFileSpans;
    const Bitboard_t blackPawnOnlyFiles = blackPawnFileSpans & ~whitePawnFileSpans;

    Bitboard_t whiteOpenRooks = boardInfo->rooks[white] & openFiles;
    Bitboard_t blackOpenRooks = boardInfo->rooks[black] & openFiles;

    Bitboard_t whiteSemiOpenRooks = boardInfo->rooks[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenRooks = boardInfo->rooks[black] & whitePawnOnlyFiles;

    TunerSerializeByFile(whiteOpenRooks, blackOpenRooks, open_rook_offset, allValues);
    TunerSerializeByFile(whiteSemiOpenRooks, blackSemiOpenRooks, semi_open_rook_offset, allValues);

    Bitboard_t whiteOpenQueens = boardInfo->queens[white] & openFiles;
    Bitboard_t blackOpenQueens = boardInfo->queens[black] & openFiles;

    Bitboard_t whiteSemiOpenQueens = boardInfo->queens[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenQueens = boardInfo->queens[black] & whitePawnOnlyFiles;

    TunerSerializeByFile(whiteOpenQueens, blackOpenQueens, open_queen_offset, allValues);
    TunerSerializeByFile(whiteSemiOpenQueens, blackSemiOpenQueens, semi_open_queen_offset, allValues);

    Bitboard_t whiteOpenKings = boardInfo->kings[white] & openFiles;
    Bitboard_t blackOpenKings = boardInfo->kings[black] & openFiles;

    Bitboard_t whiteSemiOpenKings = boardInfo->kings[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenKings = boardInfo->kings[black] & whitePawnOnlyFiles;

    TunerSerializeByFile(whiteOpenKings, blackOpenKings, open_king_offset, allValues);
    TunerSerializeByFile(whiteSemiOpenKings, blackSemiOpenKings, semi_open_king_offset, allValues);

    // PAWN STRUCTURE
    const Bitboard_t whiteFill = FileFill(boardInfo->pawns[white]);
    const Bitboard_t blackFill = FileFill(boardInfo->pawns[black]);

    const Bitboard_t whiteNeighbors = EastOne(whiteFill) | WestOne(whiteFill);
    const Bitboard_t blackNeighbors = EastOne(blackFill) | WestOne(blackFill);

    Bitboard_t wIsolated = boardInfo->pawns[white] & ~whiteNeighbors;
    Bitboard_t bIsolated = boardInfo->pawns[black] & ~blackNeighbors;

    TunerSerializeByFile(wIsolated, bIsolated, isolated_pawns_offset, allValues);
}

void FillTEntry(TEntry_t* tEntry, BoardInfo_t* boardInfo) {
    int16_t allValues[VECTOR_LENGTH] = {0};

    const Bucket_t whiteBucket = PSTBucketIndex(boardInfo, white);
    const Bucket_t blackBucket = PSTBucketIndex(boardInfo, black);

    FillPSTFeatures(allValues, whiteBucket, blackBucket, boardInfo);
    FillBonuses(allValues, whiteBucket, blackBucket, boardInfo);
    FillMobility(boardInfo, allValues);

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
static void AddPieceValComment(FILE* fp) {
    const char* names[NUM_PIECES - 1] = { "Knight", "Bishop", "Rook", "Queen", "Pawn" };

    fprintf(fp, "/*\n");
    fprintf(fp, "Average PST values for MG, EG:\n");

    for(Piece_t p = 0; p < NUM_PIECES - 1; p++) {
        int squareCount = (p == pawn) ? 2*(NUM_SQUARES - 16) : NUM_SQUARES*2;

        double mgSum = 0;
        double egSum = 0;
        for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
            for(Bucket_t bucket = 0; bucket < NUM_PST_BUCKETS; bucket++) {
                mgSum += weights[mg_phase][PSTSetOffset(bucket, p, sq, pst_offset)];
                egSum += weights[eg_phase][PSTSetOffset(bucket, p, sq, pst_offset)];
            }
        }

        int mgValue = mgSum / squareCount;
        int egValue = egSum / squareCount;

        fprintf(fp, "%s Values: %d %d\n", names[p], mgValue, egValue);
    }

    fprintf(fp, "*/\n\n");
}

static void FilePrintPST(const char* tableName, Piece_t piece, FILE* fp, int feature_offset, bool isPartOfSet) {
    fprintf(fp, "#define %s { ", tableName);
    for(Bucket_t bucket = 0; bucket < NUM_PST_BUCKETS; bucket++) {
        fprintf(fp, "{ \\\n");
        for(Square_t sq = 0; sq < NUM_SQUARES; sq++) {
            if(sq % 8 == 0) { // first row entry
                fprintf(fp, "   ");
            }

            int offset = isPartOfSet ? PSTSetOffset(bucket, piece, sq, feature_offset) : PSTSingleOffset(bucket, sq, feature_offset);
            fprintf(fp, "S(%d, %d), ",
                (int)weights[mg_phase][offset],
                (int)weights[eg_phase][offset]
            );

            if(sq % 8 == 7) { // last row entry
                fprintf(fp, "\\\n");
            }
        }
        fprintf(fp, "}, ");
    }

    fprintf(fp, "}\n\n");
}

static void PrintIndividualBonus(const char* name, int feature_offset, int count, FILE* fp) {
    fprintf(fp, "#define %s { \\\n   ", name);
    for(int i = 0; i < count; i++) { 
        fprintf(fp, "S(%d, %d), ",
            (int)weights[mg_phase][feature_offset + i],
            (int)weights[eg_phase][feature_offset + i]);
    }

    fprintf(fp, "\\\n}\n\n");
}

static void PrintBonuses(FILE* fp) {
    fprintf(fp, "#define BISHOP_PAIR_BONUS \\\n   S(%d, %d)\n\n",
        (int)weights[mg_phase][bishop_pair_offset],
        (int)weights[eg_phase][bishop_pair_offset]
    );

    FilePrintPST("PASSED_PAWN_PST", 0, fp, passed_pawn_offset, false);

    PrintIndividualBonus("BLOCKED_PASSERS", blocked_passer_offset, BLOCKED_PASSER_FEATURE_COUNT, fp);

    PrintIndividualBonus("ROOK_OPEN_FILE", open_rook_offset, OPEN_FILE_FEATURE_COUNT, fp);
    PrintIndividualBonus("ROOK_SEMI_OPEN_FILE", semi_open_rook_offset, OPEN_FILE_FEATURE_COUNT, fp);

    PrintIndividualBonus("QUEEN_OPEN_FILE", open_queen_offset, OPEN_FILE_FEATURE_COUNT, fp);
    PrintIndividualBonus("QUEEN_SEMI_OPEN_FILE", semi_open_queen_offset, OPEN_FILE_FEATURE_COUNT, fp);
    
    PrintIndividualBonus("KING_OPEN_FILE", open_king_offset, OPEN_FILE_FEATURE_COUNT, fp);
    PrintIndividualBonus("KING_SEMI_OPEN_FILE", semi_open_king_offset, OPEN_FILE_FEATURE_COUNT, fp);

    PrintIndividualBonus("ISOLATED_PAWNS", isolated_pawns_offset, ISOLATED_FEATURE_COUNT, fp);

    PrintIndividualBonus("KNIGHT_MOBILITY", knight_mobility_offset, KNIGHT_MOBILITY_FEATURE_COUNT, fp);
    PrintIndividualBonus("BISHOP_MOBILITY", bishop_mobility_offset, BISHOP_MOBILITY_FEATURE_COUNT, fp);
    PrintIndividualBonus("ROOK_MOBILITY", rook_mobility_offset, ROOK_MOBILITY_FEATURE_COUNT, fp);
    PrintIndividualBonus("QUEEN_MOBILITY", queen_mobility_offset, QUEEN_MOBILITY_FEATURE_COUNT, fp);
}

static void PrintThreats(FILE* fp) {
    fprintf(fp, "\n#define PAWN_THREAT_ON_KNIGHT S(%d, %d)\n", (int)weights[mg_phase][pawn_threat_on_knight], (int)weights[eg_phase][pawn_threat_on_knight]);
    fprintf(fp, "#define PAWN_THREAT_ON_BISHOP S(%d, %d)\n", (int)weights[mg_phase][pawn_threat_on_bishop], (int)weights[eg_phase][pawn_threat_on_bishop]);
    fprintf(fp, "#define PAWN_THREAT_ON_ROOK S(%d, %d)\n", (int)weights[mg_phase][pawn_threat_on_rook], (int)weights[eg_phase][pawn_threat_on_rook]);
    fprintf(fp, "#define PAWN_THREAT_ON_QUEEN S(%d, %d)\n\n", (int)weights[mg_phase][pawn_threat_on_queen], (int)weights[eg_phase][pawn_threat_on_queen]);

    fprintf(fp, "#define KNIGHT_THREAT_ON_BISHOP S(%d, %d)\n", (int)weights[mg_phase][knight_threat_on_bishop], (int)weights[eg_phase][knight_threat_on_bishop]);
    fprintf(fp, "#define KNIGHT_THREAT_ON_ROOK S(%d, %d)\n", (int)weights[mg_phase][knight_threat_on_rook], (int)weights[eg_phase][knight_threat_on_rook]);
    fprintf(fp, "#define KNIGHT_THREAT_ON_QUEEN S(%d, %d)\n\n", (int)weights[mg_phase][knight_threat_on_queen], (int)weights[eg_phase][knight_threat_on_queen]);

    fprintf(fp, "#define BISHOP_THREAT_ON_KNIGHT S(%d, %d)\n", (int)weights[mg_phase][bishop_threat_on_knight], (int)weights[eg_phase][bishop_threat_on_knight]);
    fprintf(fp, "#define BISHOP_THREAT_ON_ROOK S(%d, %d)\n", (int)weights[mg_phase][bishop_threat_on_rook], (int)weights[eg_phase][bishop_threat_on_rook]);
    fprintf(fp, "#define BISHOP_THREAT_ON_QUEEN S(%d, %d)\n\n", (int)weights[mg_phase][bishop_threat_on_queen], (int)weights[eg_phase][bishop_threat_on_queen]);

    fprintf(fp, "#define ROOK_THREAT_ON_QUEEN S(%d, %d)\n\n", (int)weights[mg_phase][rook_threat_on_queen], (int)weights[eg_phase][rook_threat_on_queen]);
}

static void CreateOutputFile() {
    FILE* fp = fopen("tuning_output.txt", "w");

    PrintBonuses(fp);

    PrintThreats(fp);

    AddPieceValComment(fp);

    FilePrintPST("KNIGHT_PST", knight, fp, pst_offset, true);
    FilePrintPST("BISHOP_PST", bishop, fp, pst_offset, true);
    FilePrintPST("ROOK_PST", rook, fp, pst_offset, true);
    FilePrintPST("QUEEN_PST", queen, fp, pst_offset, true);
    FilePrintPST("PAWN_PST", pawn, fp, pst_offset, true);
    FilePrintPST("KING_PST", king, fp, pst_offset, true);

    fclose(fp);
}
