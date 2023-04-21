#include "pawn_structure_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "util_macros.h"
#include "eval_constants_tools.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static Score_t passerBonus[NUM_PST_BUCKETS][NUM_SQUARES] = PASSED_PAWN_PST; 
static Score_t blockerPenalty[8] = BLOCKED_PASSERS;

static Score_t rookOpenBonus[NUM_FILES] = ROOK_OPEN_FILE;
static Score_t rookSemiOpenBonus[NUM_FILES] = ROOK_SEMI_OPEN_FILE;
static Score_t queenOpenBonus[NUM_FILES] = QUEEN_OPEN_FILE;
static Score_t queenSemiOpenBonus[NUM_FILES] = QUEEN_SEMI_OPEN_FILE;
static Score_t kingOpenBonus[NUM_FILES] = KING_OPEN_FILE;
static Score_t kingSemiOpenBonus[NUM_FILES] = KING_SEMI_OPEN_FILE;

static Score_t isolatedPawnPenalty[NUM_FILES] = ISOLATED_PAWNS;

static void ShouldCorrectlyEvaluatePassedPawns() {
    Score_t score = 0;

    FEN_t fen = "5k2/8/p1p4p/P4K2/8/1PP5/3PpP2/8 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    const Bucket_t wBucket = PSTBucketIndex(&boardInfo, white);
    const Bucket_t bBucket = PSTBucketIndex(&boardInfo, black);

    PassedPawnBonus(&boardInfo, wBucket, bBucket, &score);

    Score_t expectedScore = passerBonus[wBucket][MIRROR(f2)] - (passerBonus[bBucket][e2] + passerBonus[bBucket][h6]);

    PrintResults(score == expectedScore);
}

static void ShouldApplyBlockerPenalties() {
    Score_t score = 0;

    FEN_t fen = "8/5k2/8/3n4/3P4/1p6/1N6/3K4 w - - 4 5";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    const Bucket_t wBucket = PSTBucketIndex(&boardInfo, white);
    const Bucket_t bBucket = PSTBucketIndex(&boardInfo, black);

    PassedPawnBonus(&boardInfo, wBucket, bBucket, &score);


    Score_t expectedScore = (passerBonus[wBucket][MIRROR(d4)] - passerBonus[bBucket][b3]) + 
        (blockerPenalty[MIRROR(d5) / 8] - blockerPenalty[b2 / 8]);

    PrintResults(score == expectedScore);
}

static void ShouldGiveRookOpenFileBonus() {
    Score_t score = 0;

    FEN_t fen = "5k2/4ppr1/8/8/8/3P2P1/8/R2K4 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    OpenFileBonus(&boardInfo, &score);

    Score_t expectedScore = rookOpenBonus[0] - rookSemiOpenBonus[6];

    PrintResults(score == expectedScore);
}

static void ShouldGiveQueenOpenFileBonus() {
    Score_t score = 0;

    FEN_t fen = "5k2/4ppq1/8/8/8/3P2P1/8/Q2K4 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    OpenFileBonus(&boardInfo, &score);

    Score_t expectedScore = queenOpenBonus[0] - queenSemiOpenBonus[6];

    PrintResults(score == expectedScore);
}

static void ShouldGiveKingOpenFileBonus() {
    Score_t score = 0;

    FEN_t fen = "8/4ppk1/8/8/8/3P2P1/8/K7 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    OpenFileBonus(&boardInfo, &score);

    Score_t expectedScore = kingOpenBonus[0] - kingSemiOpenBonus[6];

    PrintResults(score == expectedScore);
}

static void ShouldGiveIsolatedPawnPenalties() {
    Score_t score = 0;

    FEN_t fen = "8/8/8/K5pp/4P3/kpP2P2/8/8 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    PawnStructure(&boardInfo, &score);

    Score_t expectedScore = isolatedPawnPenalty[2] - isolatedPawnPenalty[1];

    PrintResults(score == expectedScore);
}

void PawnStructureTDDRunner() {
    ShouldCorrectlyEvaluatePassedPawns();
    ShouldApplyBlockerPenalties();
    ShouldGiveRookOpenFileBonus();
    ShouldGiveQueenOpenFileBonus();
    ShouldGiveKingOpenFileBonus();
    ShouldGiveIsolatedPawnPenalties();
}