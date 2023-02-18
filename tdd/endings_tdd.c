#include "endings_tdd.h"
#include "debug.h"
#include "UCI.h"
#include "FEN.h"
#include "zobrist.h"
#include "make_and_unmake.h"

static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static FEN_t someFen = "1k4r1/p2n1p2/Pp1p4/3P4/1RPr3p/6P1/5P1P/R4K2 b - - 1 26";
static FEN_t checkmateFen = "1k3r2/p4p2/Pp1p4/2nP4/1RP5/6Pp/1R3P1P/3r2K1 w - - 6 30";
static FEN_t stalemateFen = "6R1/2k5/8/K2Q4/8/8/8/8 b - - 1 1";

enum {
    some_movelist_max = 13
};

// HELPERS
static bool GameEndStatusShouldBe(
    GameEndStatus_t expected,
    int moveListMaxIndex
)
{
    GameEndStatus_t actual = 
        CurrentGameEndStatus(
            &boardInfo,
            &gameStack,
            &zobristStack,
            moveListMaxIndex
        );

    return actual == expected;
}

static void MakeMoveAndAddHash(Move_t move) {
    MakeMove(&boardInfo, &gameStack, move);

    AddZobristHashToStack(&zobristStack, HashPosition(&boardInfo, &gameStack));
}

// TESTS
static void ShouldDrawWhenHalfmoveCountHits100() {
    InterpretFEN(someFen, &boardInfo, &gameStack, &zobristStack);
    gameStack.gameStates->halfmoveClock = 100;
    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldIdentifyCheckmate() {
    InterpretFEN(checkmateFen, &boardInfo, &gameStack, &zobristStack);
    PrintResults(GameEndStatusShouldBe(checkmate, movelist_empty));
}

static void ShouldIdentifyStalemate() {
    InterpretFEN(stalemateFen, &boardInfo, &gameStack, &zobristStack);
    PrintResults(GameEndStatusShouldBe(draw, movelist_empty));
}

static void ShouldRecognizeThreefoldRepetition() {
    InterpretFEN(START_FEN, &boardInfo, &gameStack, &zobristStack);

    Move_t wKnightOut;
    InitMove(&wKnightOut);
    WriteFromSquare(&wKnightOut, g1);
    WriteToSquare(&wKnightOut, f3);

    Move_t bKnightOut;
    InitMove(&bKnightOut);
    WriteFromSquare(&bKnightOut, g8);
    WriteToSquare(&bKnightOut, f6);

    Move_t wKnightBack;
    InitMove(&wKnightBack);
    WriteFromSquare(&wKnightBack, f3);
    WriteToSquare(&wKnightBack, g1);

    Move_t bKnightBack;
    InitMove(&bKnightBack);
    WriteFromSquare(&bKnightBack, f6);
    WriteToSquare(&bKnightBack, g8);

    MakeMoveAndAddHash(wKnightOut);
    MakeMoveAndAddHash(bKnightOut);
    MakeMoveAndAddHash(wKnightBack);
    MakeMoveAndAddHash(bKnightBack);
    MakeMoveAndAddHash(wKnightOut);
    MakeMoveAndAddHash(bKnightOut);
    MakeMoveAndAddHash(wKnightBack);

    bool success = GameEndStatusShouldBe(ongoing, some_movelist_max);
    MakeMoveAndAddHash(bKnightBack, black);

    success = success && GameEndStatusShouldBe(draw, some_movelist_max);

    PrintResults(success);
}

static void ShouldDrawWhenJustKings() {
    FEN_t kingsOnly = "8/8/8/6K1/3k4/8/8/8 w - - 0 1";
    InterpretFEN(kingsOnly, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldDrawKnightVsBishop() {
    FEN_t knightVsBishop = "8/4b3/8/6K1/3k4/4N3/8/8 w - - 0 1";
    InterpretFEN(knightVsBishop, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldDrawKnightVsKnight() {
    FEN_t knightVsKnight = "8/4n3/8/6K1/3k4/4N3/8/8 w - - 0 1";
    InterpretFEN(knightVsKnight, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldDrawBishopVsBishop() {
    FEN_t bishopVsBishop = "8/1b6/8/6K1/3k4/8/7B/8 w - - 0 1";
    InterpretFEN(bishopVsBishop, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldDrawOneSideKnight() {
    FEN_t oneSideKnight = "8/4k3/8/8/1K6/8/6N1/8 w - - 0 1";
    InterpretFEN(oneSideKnight, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldDrawOneSideBishop() {
    FEN_t oneSideBishop = "8/4k3/8/8/1K6/8/6b1/8 w - - 0 1";
    InterpretFEN(oneSideBishop, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max));
}

static void ShouldNotDrawTwoMinorPieces() {
    FEN_t twoSideBishop = "8/4k3/8/8/1K2bb2/8/8/8 w - - 0 1";
    InterpretFEN(twoSideBishop, &boardInfo, &gameStack, &zobristStack);

    PrintResults(GameEndStatusShouldBe(ongoing, some_movelist_max));
}

void EndingsTDDRunner() {
    ShouldDrawWhenHalfmoveCountHits100();
    ShouldIdentifyCheckmate();
    ShouldIdentifyStalemate();
    ShouldRecognizeThreefoldRepetition();
    ShouldDrawWhenJustKings();
    ShouldDrawKnightVsBishop();
    ShouldDrawKnightVsKnight();
    ShouldDrawBishopVsBishop();
    ShouldDrawOneSideKnight();
    ShouldDrawOneSideBishop();
    ShouldNotDrawTwoMinorPieces();
}