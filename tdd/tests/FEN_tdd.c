#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "debug.h"
#include "FEN_tdd.h"
#include "bitboards.h"
#include "board_info.h"
#include "zobrist.h"
#include "perft_table.h"

#define COMPLEX_FEN "r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R w K - 34 56"

static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void TestSetup() {
    InitGameStack(&gameStack);
}

// HELPERS
static void InitStartFENExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
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
    });

    AddStartingGameState(&gameStack);
    *expectedState = ReadCurrentGameState(&gameStack);
}

static void InitComplexFENExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
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
    });

    GameState_t* state = GetEmptyNextGameState(&gameStack);
    state->halfmoveClock = 34;
    state->castleSquares[white] = white_kingside_castle_bb;
    *expectedState = *state;
}

// TESTS
static void StartFENInterpretedCorrectly() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;

    InitStartFENExpectedInfo(&expectedInfo, &expectedState);
    InterpretFEN(START_FEN, &info, &gameStack, &zobristStack);

    PrintResults(CompareInfo(&info, &expectedInfo) && CompareState(&expectedState, &gameStack));
}

static void ComplexFENInterpretedCorrectly() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    
    InitComplexFENExpectedInfo(&expectedInfo, &expectedState);
    InterpretFEN(COMPLEX_FEN, &info, &gameStack, &zobristStack);

    PrintResults(CompareInfo(&info, &expectedInfo) && CompareState(&expectedState, &gameStack));
}

static bool FENsMatch(FEN_t expected, FEN_t actual) {
    int len = strlen(expected);
    int finalIndex = len-1;
    while(actual[finalIndex] != ' ') {
        finalIndex--;
    }

    for(int i = 0; i < finalIndex; i++) {
        if(actual[i] != expected[i]) {
            printf("Expected %s, got %s\n", expected, actual);
            return false;
        }
    }
    return true;
}

static void ToFENWorks() {
    FEN_t fenList[] = { PERFT_TEST_TABLE(EXPAND_AS_FEN_ARRAY) };
    BoardInfo_t info;

    bool success = true;
    for(int i = 0; i < NUM_PERFT_ENTRIES; i++) {
        FEN_t fen = fenList[i];
        InterpretFEN(fen, &info, &gameStack, &zobristStack);
        char result[2000];
        BoardToFEN(&info, &gameStack, result);

        if(!FENsMatch(fen, result)) {
            success = false;
            break;
        }
    }

    PrintResults(success);
}

void FENTDDRunner() {
    StartFENInterpretedCorrectly();
    ComplexFENInterpretedCorrectly();
    ToFENWorks();
}   