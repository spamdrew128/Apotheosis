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
    int moveListMaxIndex,
    Color_t colorToMove
)
{
    GameState_t gameState = ReadCurrentGameState(&gameStack);
    GameEndStatus_t actual = 
        CurrentGameEndStatus(
            &boardInfo,
            &gameStack,
            &zobristStack,
            HashPosition(&boardInfo, &gameState, colorToMove),
            moveListMaxIndex,
            colorToMove
        );

    return actual == expected;
}

static void AddHashAndMakeMove(Move_t move, Color_t moveColor) {
    GameState_t gameState = ReadCurrentGameState(&gameStack);
    AddZobristHashToStack(&zobristStack, HashPosition(&boardInfo, &gameState, moveColor));

    MakeMove(&boardInfo, &gameStack, move, moveColor);
}

// TESTS
static void ShouldDrawWhenHalfmoveCountHits100() {
    InitZobristStack(&zobristStack);
    Color_t color = InterpretFEN(someFen, &boardInfo, &gameStack);
    gameStack.gameStates->halfmoveClock = 100;
    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldIdentifyCheckmate() {
    InitZobristStack(&zobristStack);
    Color_t color = InterpretFEN(checkmateFen, &boardInfo, &gameStack);
    PrintResults(GameEndStatusShouldBe(checkmate, movelist_empty, color));
}

static void ShouldIdentifyStalemate() {
    InitZobristStack(&zobristStack);
    Color_t color = InterpretFEN(stalemateFen, &boardInfo, &gameStack);
    PrintResults(GameEndStatusShouldBe(draw, movelist_empty, color));
}

static void ShouldRecognizeThreefoldRepetition() {
    InitZobristStack(&zobristStack);
    InterpretFEN(START_FEN, &boardInfo, &gameStack);

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

    AddHashAndMakeMove(wKnightOut, white);
    AddHashAndMakeMove(bKnightOut, black);
    AddHashAndMakeMove(wKnightBack, white);
    AddHashAndMakeMove(bKnightBack, black);
    AddHashAndMakeMove(wKnightOut, white);
    AddHashAndMakeMove(bKnightOut, black);
    AddHashAndMakeMove(wKnightBack, white);

    bool success = GameEndStatusShouldBe(ongoing, some_movelist_max, black);
    AddHashAndMakeMove(bKnightBack, black);

    success = success && GameEndStatusShouldBe(draw, some_movelist_max, white);

    PrintResults(success);
}

static void ShouldDrawWhenJustKings() {
    InitZobristStack(&zobristStack);
    FEN_t kingsOnly = "8/8/8/6K1/3k4/8/8/8 w - - 0 1";
    Color_t color = InterpretFEN(kingsOnly, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldDrawKnightVsBishop() {
    InitZobristStack(&zobristStack);
    FEN_t knightVsBishop = "8/4b3/8/6K1/3k4/4N3/8/8 w - - 0 1";
    Color_t color = InterpretFEN(knightVsBishop, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldDrawKnightVsKnight() {
    InitZobristStack(&zobristStack);
    FEN_t knightVsKnight = "8/4n3/8/6K1/3k4/4N3/8/8 w - - 0 1";
    Color_t color = InterpretFEN(knightVsKnight, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldDrawBishopVsBishop() {
    InitZobristStack(&zobristStack);
    FEN_t bishopVsBishop = "8/1b6/8/6K1/3k4/8/7B/8 w - - 0 1";
    Color_t color = InterpretFEN(bishopVsBishop, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldDrawOneSideKnight() {
    InitZobristStack(&zobristStack);
    FEN_t oneSideKnight = "8/4k3/8/8/1K6/8/6N1/8 w - - 0 1";
    Color_t color = InterpretFEN(oneSideKnight, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldDrawOneSideBishop() {
    InitZobristStack(&zobristStack);
    FEN_t oneSideBishop = "8/4k3/8/8/1K6/8/6b1/8 w - - 0 1";
    Color_t color = InterpretFEN(oneSideBishop, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(draw, some_movelist_max, color));
}

static void ShouldNotDrawTwoMinorPieces() {
    InitZobristStack(&zobristStack);
    FEN_t twoSideBishop = "8/4k3/8/8/1K2bb2/8/8/8 w - - 0 1";
    Color_t color = InterpretFEN(twoSideBishop, &boardInfo, &gameStack);

    PrintResults(GameEndStatusShouldBe(ongoing, some_movelist_max, color));
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