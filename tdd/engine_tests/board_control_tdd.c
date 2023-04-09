#include "board_control_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "util_macros.h"
#include "eval_constants_tools.h"
#include "eval_helpers.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static Score_t boardControlBonus[NUM_PST_BUCKETS][NUM_SQUARES] = BOARD_CONTROL_BONUSES;

static void ShouldGiveCorrectScore() {
    Score_t score = 0;

    FEN_t fen = "8/8/5p2/1k1B4/7P/1K3b2/6q1/8 w - - 0 1";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    const Bucket_t wBucket = PSTBucketIndex(&boardInfo, white);
    const Bucket_t bBucket = PSTBucketIndex(&boardInfo, black);

    BoardControl(&boardInfo, wBucket, bBucket, &score);

    Bitboard_t wControlled = CreateBitboard(8, a8,g8,b7,f7,c6,e6,c4,b3);
    Bitboard_t bControlled = (g_file | rank_2 | CreateBitboard(12, f1,g1,h1,h3,f3,e4,d5,e5,d1,e2,g4,h5)) & ~CreateBitboard(2, g3,g8);

    Score_t expectedScore = 0;
    SerializeBySquare(wControlled, bControlled, wBucket, bBucket, &expectedScore, boardControlBonus);

    PrintResults(score == expectedScore);
}

void BoardControlTDDRunner() {
    ShouldGiveCorrectScore();
}