#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "FEN_tdd.h"
#include "bitboards.h"
#include "board_info.h"

#define COMPLEX_FEN "r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R w K e3 34 56"

static GameStack_t stack;

static void TestSetup() {
    InitGameStack(&stack);
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

    AddStartingGameState(&stack);
    *expectedState = ReadCurrentGameState(&stack);
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

    GameState_t* state = GetEmptyNextGameState(&stack);
    state->halfmoveClock = 34;
    SetKingsideCastleRights(&state->castleRights, white);
    state->enPassantSquares = CreateBitboard(1, e3);
    *expectedState = *state;
}

// TESTS
static void StartFENInterpretedCorrectly() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;

    InitStartFENExpectedInfo(&expectedInfo, &expectedState);
    InterpretFEN(START_FEN, &info, &stack);

    PrintResults(CompareInfo(&info, &expectedInfo) && CompareState(&expectedState, &stack));
}

static void ComplexFENInterpretedCorrectly() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    
    InitComplexFENExpectedInfo(&expectedInfo, &expectedState);
    InterpretFEN(COMPLEX_FEN, &info, &stack);

    PrintResults(CompareInfo(&info, &expectedInfo) && CompareState(&expectedState, &stack));
}

void FENTDDRunner() {
    StartFENInterpretedCorrectly();
    ComplexFENInterpretedCorrectly();
}   