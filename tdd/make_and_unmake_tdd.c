#include "make_and_unmake_tdd.h"
#include "movegen.h"
#include "game_state.h"
#include "board_info.h"
#include "debug.h"

enum {
    some_halfmove_clock = 32
};

// HELPERS
static GameStack_t stack;

static void TestSetup() {
    InitGameStack(&stack);
}

// r3k2r/8/8/8/8/8/8/R3K2R
static void InitAllCastlingLegalInfo(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, e1);
        info->rooks[white] = CreateBitboard(2, a1,h1);

        info->kings[black] = CreateBitboard(1, e8);
        info->rooks[black] = CreateBitboard(2, a8,h8);
    });

    AddStartingGameState(&stack);
    stack.gameStates[stack.top].boardInfo = *info;
}

// r3k2r/8/8/8/8/8/8/R4RK1
static void InitKingsideCastleExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, g1);
        expectedInfo->rooks[white] = CreateBitboard(2, a1,f1);

        expectedInfo->kings[black] = CreateBitboard(1, e8);
        expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);
    });

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    ResetAllRights(&nextState.castleRights, white);
    *expectedState = nextState;
}

// r3k2r/8/8/8/8/8/8/R4RK1
static void InitQueensideCastleExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, c1);
        expectedInfo->rooks[white] = CreateBitboard(2, d1,h1);

        expectedInfo->kings[black] = CreateBitboard(1, e8);
        expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);
    });

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    ResetAllRights(&nextState.castleRights, white);
    *expectedState = nextState;
}

// 8/4P3/7K/8/8/7k/2p5/1Q6
static void InitPromotionPostionInfo(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, h6);
        info->pawns[white] = CreateBitboard(1, e7);
        info->queens[white] = CreateBitboard(1, b1);

        info->kings[black] = CreateBitboard(1, h3);
        info->pawns[black] = CreateBitboard(1, c2);
    });

    AddStartingGameState(&stack);
    stack.gameStates[stack.top].boardInfo = *info;
}

// 8/4P3/7K/8/8/7k/2p5/1Q6
static void InitExpectedQuietPromotionPostionInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, h6);
        expectedInfo->queens[white] = CreateBitboard(2, b1,e8);

        expectedInfo->kings[black] = CreateBitboard(1, h3);
        expectedInfo->pawns[black] = CreateBitboard(1, c2);
    });

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    nextState.halfmoveClock = 0;
    *expectedState = nextState;
}

// 8/4P3/7K/8/8/7k/2p5/1Q6
static void InitExpectedCapturePromotionPostionInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, h6);
        expectedInfo->pawns[white] = CreateBitboard(1, e7);

        expectedInfo->kings[black] = CreateBitboard(1, h3);
        expectedInfo->knights[black] = CreateBitboard(1, b1);
    });

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    nextState.halfmoveClock = 0;
    nextState.capturedPiece = queen;
    ResetAllRights(&nextState.castleRights, white);
    *expectedState = nextState;
}

// 8/8/4k3/6Pp/2Pp1K2/8/8/8
static void InitBothSidesEnPassantInfo(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, f4);
        info->pawns[white] = CreateBitboard(2, c4,g5);

        info->kings[black] = CreateBitboard(1, e6);
        info->pawns[black] = CreateBitboard(2, d4,h5);
    });

    GameState_t* state = GetEmptyNextGameState(&stack);
    state->halfmoveClock = some_halfmove_clock;
    state->enPassantSquares = CreateBitboard(2, c3,h6);
    state->capturedPiece = none_type;
    state->boardInfo = *info;
}

static void InitSideEnPassantExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState, Color_t moveColor) {
    if(moveColor == white) {
        InitTestInfo(expectedInfo, {
            expectedInfo->kings[white] = CreateBitboard(1, f4);
            expectedInfo->pawns[white] = CreateBitboard(2, c4,h6);

            expectedInfo->kings[black] = CreateBitboard(1, e6);
            expectedInfo->pawns[black] = CreateBitboard(1, d4);
        });
    } else {
        InitTestInfo(expectedInfo, {
            expectedInfo->kings[white] = CreateBitboard(1, f4);
            expectedInfo->pawns[white] = CreateBitboard(1 ,g5);

            expectedInfo->kings[black] = CreateBitboard(1, e6);
            expectedInfo->pawns[black] = CreateBitboard(2, c3,h5);
        });
    }

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    nextState.halfmoveClock = 0;
    nextState.enPassantSquares = empty_set;
    nextState.capturedPiece = pawn;
    *expectedState = nextState;
}

// 8/3p4/k7/8/4n3/8/1K3P2/8
static void InitNormalPosition(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, b2);
        info->pawns[white] = CreateBitboard(1, f2);

        info->kings[black] = CreateBitboard(1, a6);
        info->pawns[black] = CreateBitboard(1, d7);
        info->knights[black] = CreateBitboard(1, e4);
    });

    GameState_t* state = GetEmptyNextGameState(&stack);
    state->halfmoveClock = some_halfmove_clock;
    state->enPassantSquares = empty_set;
    state->capturedPiece = none_type;
    state->boardInfo = *info;
}

static void InitNormalQuietExpected(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, c3);
        expectedInfo->pawns[white] = CreateBitboard(1, f2);

        expectedInfo->kings[black] = CreateBitboard(1, a6);
        expectedInfo->pawns[black] = CreateBitboard(1, d7);
        expectedInfo->knights[black] = CreateBitboard(1, e4);
    });

    GameState_t state = ReadDefaultNextGameState(&stack);
    *expectedState = state;
}

static void InitPawnDoublePushExpected(BoardInfo_t* expectedInfo, GameState_t* expectedState, Color_t moveColor) {
    GameState_t state = ReadDefaultNextGameState(&stack);
    state.halfmoveClock = 0;

    if(moveColor == white) {
        InitTestInfo(expectedInfo, {
            expectedInfo->kings[white] = CreateBitboard(1, b2);
            expectedInfo->pawns[white] = CreateBitboard(1, f4);

            expectedInfo->kings[black] = CreateBitboard(1, a6);
            expectedInfo->pawns[black] = CreateBitboard(1, d7);
            expectedInfo->knights[black] = CreateBitboard(1, e4);
        });

        state.enPassantSquares = CreateBitboard(1, f3);
    } else {
        InitTestInfo(expectedInfo, {
            expectedInfo->kings[white] = CreateBitboard(1, b2);
            expectedInfo->pawns[white] = CreateBitboard(1, f2);

            expectedInfo->kings[black] = CreateBitboard(1, a6);
            expectedInfo->pawns[black] = CreateBitboard(1, d5);
            expectedInfo->knights[black] = CreateBitboard(1, e4);
        });   

        state.enPassantSquares = CreateBitboard(1, d6);
    }

    *expectedState = state;
}

static void InitNormalCaptureExpectedPosition(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, b2);

        expectedInfo->kings[black] = CreateBitboard(1, a6);
        expectedInfo->pawns[black] = CreateBitboard(1, d7);
        expectedInfo->knights[black] = CreateBitboard(1, f2);
    });

    GameState_t state = ReadDefaultNextGameState(&stack);
    state.halfmoveClock = 0;
    state.capturedPiece = pawn;
    *expectedState = state;
}

// r3k2r/8/5b2/8/8/8/8/R3K2R
static void InitBreakCastlingPosition(BoardInfo_t* info) {
    InitAllCastlingLegalInfo(info);
    info->bishops[black] = CreateBitboard(1, f6);
    AddPieceToMailbox(info, f6, bishop);
    stack.gameStates[stack.top].boardInfo = *info;
}

static void InitBreakCastlingPositionExpected(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, e1);
        expectedInfo->rooks[white] = CreateBitboard(1, h1);

        expectedInfo->kings[black] = CreateBitboard(1, e8);
        expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);
        expectedInfo->bishops[black] = CreateBitboard(1, a1);
    });

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    nextState.halfmoveClock = 0;
    ResetQueensideCastleRights(&nextState.castleRights, white);
    
    nextState.capturedPiece = rook;
    *expectedState = nextState;
}

// rn2k3/1P6/8/8/8/1K6/8/8
static void InitPromotionCastleBreakPosition(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, b3);
        info->pawns[white] = CreateBitboard(1, b7);

        info->kings[black] = CreateBitboard(1, e8);
        info->rooks[black] = CreateBitboard(1, a8);
        info->knights[black] = CreateBitboard(1, b8);
    });

    GameState_t* state = GetEmptyNextGameState(&stack);
    state->halfmoveClock = some_halfmove_clock;
    state->enPassantSquares = empty_set;
    SetQueensideCastleRights(&state->castleRights, black);
    state->boardInfo = *info;
    state->capturedPiece = none_type;
}

// 4k1nr/7P/8/8/8/1K6/8/8
static void InitPromotionCastleBreakPositionExpected(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, b3);
        expectedInfo->knights[white] = CreateBitboard(1, a8);

        expectedInfo->kings[black] = CreateBitboard(1, e8);
        expectedInfo->knights[black] = CreateBitboard(1, b8);
    });

    GameState_t nextState = ReadDefaultNextGameState(&stack);
    nextState.halfmoveClock = 0;
    InitCastleRightsToZero(&nextState.castleRights);
    nextState.capturedPiece = rook;
    *expectedState = nextState;
}

// TESTS
static void ShouldCastleKingside() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitAllCastlingLegalInfo(&info);
    InitKingsideCastleExpectedInfo(&expectedInfo, &expectedState);

    Move_t ksCastle;
    InitMove(&ksCastle);
    WriteFromSquare(&ksCastle, LSB(info.kings[white]));
    WriteToSquare(&ksCastle, LSB(white_kingside_castle_bb));
    WriteSpecialFlag(&ksCastle, castle_flag);

    MakeMove(&info, &stack, ksCastle, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldCastleQueenside() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitAllCastlingLegalInfo(&info);
    InitQueensideCastleExpectedInfo(&expectedInfo, &expectedState);

    Move_t qsCastle;
    InitMove(&qsCastle);
    WriteFromSquare(&qsCastle, LSB(info.kings[white]));
    WriteToSquare(&qsCastle, LSB(white_queenside_castle_bb));
    WriteSpecialFlag(&qsCastle, castle_flag);

    MakeMove(&info, &stack, qsCastle, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldQuietPromote() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitPromotionPostionInfo(&info);
    InitExpectedQuietPromotionPostionInfo(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, e7);
    WriteToSquare(&move, e8);
    WritePromotionPiece(&move, queen);
    WriteSpecialFlag(&move, promotion_flag);

    MakeMove(&info, &stack, move, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldCapturePromote() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitPromotionPostionInfo(&info);
    InitExpectedCapturePromotionPostionInfo(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, c2);
    WriteToSquare(&move, b1);
    WritePromotionPiece(&move, knight);
    WriteSpecialFlag(&move, promotion_flag);

    MakeMove(&info, &stack, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldWhiteEnPassant() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitBothSidesEnPassantInfo(&info);
    InitSideEnPassantExpectedInfo(&expectedInfo, &expectedState, white);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, g5);
    WriteToSquare(&move, h6);
    WriteSpecialFlag(&move, en_passant_flag);

    MakeMove(&info, &stack, move, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldBlackEnPassant() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitBothSidesEnPassantInfo(&info);
    InitSideEnPassantExpectedInfo(&expectedInfo, &expectedState, black);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, d4);
    WriteToSquare(&move, c3);
    WriteSpecialFlag(&move, en_passant_flag);

    MakeMove(&info, &stack, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldMakeNormalQuietMoves() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitNormalPosition(&info);
    InitNormalQuietExpected(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, b2);
    WriteToSquare(&move, c3);

    MakeMove(&info, &stack, move, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldDoublePushPawns(Color_t moveColor) {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitNormalPosition(&info);
    InitPawnDoublePushExpected(&expectedInfo, &expectedState, moveColor);

    Move_t move;
    InitMove(&move);
    if(moveColor == white) {
        WriteFromSquare(&move, f2);
        WriteToSquare(&move, f4);
        MakeMove(&info, &stack, move, white);
    } else {
        WriteFromSquare(&move, d7);
        WriteToSquare(&move, d5);
        MakeMove(&info, &stack, move, black);
    }

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldMakeNormalCaptures() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitNormalPosition(&info);
    InitNormalCaptureExpectedPosition(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, e4);
    WriteToSquare(&move, f2);

    MakeMove(&info, &stack, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void CapturingRookShouldRemoveCastleSquares() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitBreakCastlingPosition(&info);
    InitBreakCastlingPositionExpected(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, f6);
    WriteToSquare(&move, a1);

    MakeMove(&info, &stack, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

static void PromotionCaptureShouldRemoveCastleSquares() {
    TestSetup();
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitPromotionCastleBreakPosition(&info);
    InitPromotionCastleBreakPositionExpected(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, b7);
    WriteToSquare(&move, a8);
    WritePromotionPiece(&move, knight);
    WriteSpecialFlag(&move, promotion_flag);

    MakeMove(&info, &stack, move, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState, &stack);

    PrintResults(infoMatches && stateMatches);
}

void MakeMoveTDDRunner() {
    ShouldCastleKingside();
    ShouldCastleQueenside();

    ShouldQuietPromote();
    ShouldCapturePromote();

    ShouldWhiteEnPassant();
    ShouldBlackEnPassant();

    ShouldMakeNormalQuietMoves();
    ShouldDoublePushPawns(white);
    ShouldDoublePushPawns(black);
    ShouldMakeNormalCaptures();

    CapturingRookShouldRemoveCastleSquares();
    PromotionCaptureShouldRemoveCastleSquares();
}

// UMAKE HELPERS
static bool GenericTestUnmake(BoardInfo_t* startInfo, Move_t move, Color_t moveColor) {
    BoardInfo_t expectedInfo = *startInfo;
    GameState_t originalState;
    originalState.halfmoveClock = ReadHalfmoveClock(&stack);
    originalState.castleRights = ReadCastleRights(&stack);
    originalState.enPassantSquares = ReadEnPassantSquares(&stack);
    originalState.capturedPiece = ReadCapturedPiece(&stack);

    MakeMove(startInfo, &stack, move, moveColor);
    UnmakeMove(startInfo, &stack);

    bool infoMatches = CompareInfo(startInfo, &expectedInfo);
    bool stateMatches = CompareState(&originalState, &stack);

    return infoMatches && stateMatches;
}

static void ShouldCastleKingsideUnmake(){
    TestSetup();
    BoardInfo_t info;
    InitAllCastlingLegalInfo(&info);

    Move_t ksCastle;
    InitMove(&ksCastle);
    WriteFromSquare(&ksCastle, LSB(info.kings[white]));
    WriteToSquare(&ksCastle, LSB(white_kingside_castle_bb));
    WriteSpecialFlag(&ksCastle, castle_flag);

    PrintResults(GenericTestUnmake(&info, ksCastle, white));
}

static void ShouldCastleQueensideUnmake() {
    TestSetup();
    BoardInfo_t info;
    InitAllCastlingLegalInfo(&info);

    Move_t qsCastle;
    InitMove(&qsCastle);
    WriteFromSquare(&qsCastle, LSB(info.kings[white]));
    WriteToSquare(&qsCastle, LSB(white_queenside_castle_bb));
    WriteSpecialFlag(&qsCastle, castle_flag);

    PrintResults(GenericTestUnmake(&info, qsCastle, white));
}

static void ShouldQuietPromoteUnmake() {
    TestSetup();
    BoardInfo_t info;
    InitPromotionPostionInfo(&info);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, e7);
    WriteToSquare(&move, e8);
    WritePromotionPiece(&move, queen);
    WriteSpecialFlag(&move, promotion_flag);

    PrintResults(GenericTestUnmake(&info, move, white));
}

static void ShouldCapturePromoteUnmake() {
    TestSetup();
    BoardInfo_t info;
    InitPromotionPostionInfo(&info);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, c2);
    WriteToSquare(&move, b1);
    WritePromotionPiece(&move, knight);
    WriteSpecialFlag(&move, promotion_flag);

    PrintResults(GenericTestUnmake(&info, move, black));
}

static void ShouldWhiteEnPassantUnmake() {
    TestSetup();
    BoardInfo_t info;
    InitBothSidesEnPassantInfo(&info);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, g5);
    WriteToSquare(&move, h6);
    WriteSpecialFlag(&move, en_passant_flag);

    PrintResults(GenericTestUnmake(&info, move, white));
}

static void ShouldBlackEnPassantUnmake() {
    TestSetup();
    BoardInfo_t info;
    InitBothSidesEnPassantInfo(&info);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, d4);
    WriteToSquare(&move, c3);
    WriteSpecialFlag(&move, en_passant_flag);

    PrintResults(GenericTestUnmake(&info, move, black));
}

static void ShouldUnmakeNormalQuietMoves() {
    TestSetup();
    BoardInfo_t info;
    InitNormalPosition(&info);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, b2);
    WriteToSquare(&move, c3);

    PrintResults(GenericTestUnmake(&info, move, white));
}

static void ShouldUnmakeNormalCaptures() {
    TestSetup();
    BoardInfo_t info;
    InitNormalPosition(&info);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, e4);
    WriteToSquare(&move, f2);

    PrintResults(GenericTestUnmake(&info, move, black));
}

void UnmakeMoveTDDRunner() {
    ShouldCastleKingsideUnmake();
    ShouldCastleQueensideUnmake();

    ShouldQuietPromoteUnmake();
    ShouldCapturePromoteUnmake();

    ShouldWhiteEnPassantUnmake();
    ShouldBlackEnPassantUnmake();

    ShouldUnmakeNormalQuietMoves();
    ShouldUnmakeNormalCaptures();
}