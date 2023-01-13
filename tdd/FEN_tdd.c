#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "FEN_tdd.h"
#include "FEN.h"
#include "bitboards.h"

#define COMPLEX_FEN "r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R"

// HELPERS
static void InitStartFENExpectedInfo(BoardInfo_t* expectedInfo) {
    expectedInfo->pawns[white] = rank_2;
    expectedInfo->knights[white] = CreateBitboard(2, b1,g1);
    expectedInfo->bishops[white] = CreateBitboard(2, c1,f1);
    expectedInfo->rooks[white] = CreateBitboard(2, a1,h1);
    expectedInfo->queens[white] = CreateBitboard(1, d1);
    expectedInfo->kings[white] = CreateBitboard(1, e1);

    expectedInfo->pawns[black] = rank_7;
    expectedInfo->knights[black] = CreateBitboard(2, b8,g8);
    expectedInfo->bishops[black] = CreateBitboard(2, c8,f8);
    expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);
    expectedInfo->queens[black] = CreateBitboard(1, d8);
    expectedInfo->kings[black] = CreateBitboard(1, e8);

    UpdateAllPieces(expectedInfo);
    UpdateEmpty(expectedInfo);
}

static void InitComplexFENExpectedInfo(BoardInfo_t* expectedInfo) {
    expectedInfo->pawns[white] = CreateBitboard(7, a2,b2,c2,d5,e4,f2,g5);
    expectedInfo->knights[white] = CreateBitboard(2, c3,f3);
    expectedInfo->bishops[white] = CreateBitboard(1, f1);
    expectedInfo->rooks[white] = CreateBitboard(2, d1,h1);
    expectedInfo->queens[white] = CreateBitboard(1, h6);
    expectedInfo->kings[white] = CreateBitboard(1, c1);

    expectedInfo->pawns[black] = CreateBitboard(8, a7,b7,c6,d6,e5,f7,g6,h7);
    expectedInfo->knights[black] = CreateBitboard(2, e7,f6);
    expectedInfo->bishops[black] = CreateBitboard(1, c8);
    expectedInfo->rooks[black] = CreateBitboard(2, a8,f8);
    expectedInfo->queens[black] = CreateBitboard(1, e8);
    expectedInfo->kings[black] = CreateBitboard(1, g8);

    UpdateAllPieces(expectedInfo);
    UpdateEmpty(expectedInfo);
}

static bool CompareInfo(BoardInfo_t* info, BoardInfo_t* expectedInfo) {
    bool success = true;
    for(int i = 0; i < 2; i++) {
        success = success &&
            (info->allPieces[i] == expectedInfo->allPieces[i]) &&
            (info->pawns[i] == expectedInfo->pawns[i]) &&
            (info->knights[i] == expectedInfo->knights[i]) &&
            (info->bishops[i] == expectedInfo->bishops[i]) &&
            (info->rooks[i] == expectedInfo->rooks[i]) &&
            (info->queens[i] == expectedInfo->queens[i]) &&
            (info->kings[i] == expectedInfo->kings[i]);
    }

    success = success && (info->empty == expectedInfo->empty);

    return success;
}

// TESTS
static void StartFENInterpretedCorrectly() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;

    InitBoardInfo(&info);
    InterpretFEN(START_FEN, &info);
    InitStartFENExpectedInfo(&expectedInfo);

    PrintResults(CompareInfo(&info, &expectedInfo));
}

static void ComplexFENInterpretedCorrectly() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    
    InitBoardInfo(&info);
    InterpretFEN(COMPLEX_FEN, &info);
    InitComplexFENExpectedInfo(&expectedInfo);

    PrintResults(CompareInfo(&info, &expectedInfo));
}

void FENTDDRunner() {
    StartFENInterpretedCorrectly();
    ComplexFENInterpretedCorrectly();
}   