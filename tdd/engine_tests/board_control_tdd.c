#include "board_control_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "util_macros.h"
#include "eval_constants_tools.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void ShouldGiveCorrectScore() {
    Score_t score = 0;

    FEN_t fen = "5rk1/p4pp1/1pQ4p/3NP3/2BK1P2/6P1/PPP2n1n/8 b - - 5 26";
    InterpretFEN(fen, &boardInfo, &gameStack, &zobristStack);

    const Bucket_t wBucket = PSTBucketIndex(&boardInfo, white);
    const Bucket_t bBucket = PSTBucketIndex(&boardInfo, black);

    BoardControl(&boardInfo, wBucket, bBucket, &score);
}

void BoardControlTDDRunner() {
    ShouldGiveCorrectScore();
}