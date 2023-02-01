#include "make_and_unmake_tdd.h"
#include "movegen.h"
#include "game_state.h"
#include "board_info.h"
#include "debug.h"

enum {
    some_halfmove_clock = 32
};

// HELPERS
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

    for(int i = 0; i < NUM_SQUARES; i++) {
        success = success && 
            PieceOnSquare(info, i) == PieceOnSquare(expectedInfo, i);
    }

    return success;
}

static bool CompareState(GameState_t* expectedState) {
    return
        (ReadHalfmoveClock() == expectedState->halfmoveClock) &&
        (ReadEnPassantSquares() == expectedState->enPassantSquares) &&
        (ReadCastleSquares(white) == expectedState->castleSquares[white]) &&
        (ReadCastleSquares(black) == expectedState->castleSquares[black]);
}

// r3k2r/8/8/8/8/8/8/R3K2R
static void InitAllCastlingLegalInfo(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, e1);
        info->rooks[white] = CreateBitboard(2, a1,h1);

        info->kings[black] = CreateBitboard(1, e8);
        info->rooks[black] = CreateBitboard(2, a8,h8);
    });

    AddStartingGameState();
}

// r3k2r/8/8/8/8/8/8/R4RK1
static void InitKingsideCastleExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, g1);
        expectedInfo->rooks[white] = CreateBitboard(2, a1,f1);

        expectedInfo->kings[black] = CreateBitboard(1, e8);
        expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);
    });

    GameState_t temp = ReadDefaultNextGameState();
    expectedState->halfmoveClock = temp.halfmoveClock;
    expectedState->castleSquares[white] = empty_set;
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = temp.enPassantSquares;
}

// r3k2r/8/8/8/8/8/8/R4RK1
static void InitQueensideCastleExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, c1);
        expectedInfo->rooks[white] = CreateBitboard(2, d1,h1);

        expectedInfo->kings[black] = CreateBitboard(1, e8);
        expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);
    });

    GameState_t temp = ReadDefaultNextGameState();
    expectedState->halfmoveClock = temp.halfmoveClock;
    expectedState->castleSquares[white] = empty_set;
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = temp.enPassantSquares;
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

    AddStartingGameState();
}

// 8/4P3/7K/8/8/7k/2p5/1Q6
static void InitExpectedQuietPromotionPostionInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, h6);
        expectedInfo->queens[white] = CreateBitboard(2, b1,e8);

        expectedInfo->kings[black] = CreateBitboard(1, h3);
        expectedInfo->pawns[black] = CreateBitboard(1, c2);
    });

    GameState_t temp = ReadDefaultNextGameState();
    expectedState->halfmoveClock = 0;
    expectedState->castleSquares[white] = temp.castleSquares[white];
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = temp.enPassantSquares;
}

// 8/4P3/7K/8/8/7k/2p5/1Q6
static void InitExpectedCapturePromotionPostionInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, h6);
        expectedInfo->pawns[white] = CreateBitboard(1, e7);

        expectedInfo->kings[black] = CreateBitboard(1, h3);
        expectedInfo->knights[black] = CreateBitboard(1, b1);
    });

    GameState_t temp = ReadDefaultNextGameState();
    expectedState->halfmoveClock = 0;
    expectedState->castleSquares[white] = temp.castleSquares[white];
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = temp.enPassantSquares;
}

// 8/8/4k3/6Pp/2Pp1K2/8/8/8
static void InitBothSidesEnPassantInfo(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, f4);
        info->pawns[white] = CreateBitboard(2, c4,g5);

        info->kings[black] = CreateBitboard(1, e6);
        info->pawns[black] = CreateBitboard(2, d4,h5);
    });

    GameState_t* state = GetUninitializedNextGameState();
    state->halfmoveClock = some_halfmove_clock;
    state->castleSquares[white] = empty_set;
    state->castleSquares[black] = empty_set;
    state->enPassantSquares = CreateBitboard(2, c3,h6);
}

static void InitSideEnPassantExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState, Color_t color) {
    if(color == white) {
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

    GameState_t temp = ReadDefaultNextGameState();
    expectedState->halfmoveClock = 0;
    expectedState->castleSquares[white] = temp.castleSquares[white];
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = empty_set;
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

    GameState_t* state = GetUninitializedNextGameState();
    state->halfmoveClock = some_halfmove_clock;
    state->enPassantSquares = empty_set;
    state->castleSquares[white] = empty_set;
    state->castleSquares[black] = empty_set;
}

static void InitNormalQuietExpected(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitTestInfo(expectedInfo, {
        expectedInfo->kings[white] = CreateBitboard(1, c3);
        expectedInfo->pawns[white] = CreateBitboard(1, f2);

        expectedInfo->kings[black] = CreateBitboard(1, a6);
        expectedInfo->pawns[black] = CreateBitboard(1, d7);
        expectedInfo->knights[black] = CreateBitboard(1, e4);
    });

    GameState_t state = ReadDefaultNextGameState();
    *expectedState = state;
}

static void InitPawnDoublePushExpected(BoardInfo_t* expectedInfo, GameState_t* expectedState, Color_t color) {
    GameState_t state = ReadDefaultNextGameState();
    state.halfmoveClock = 0;

    if(color == white) {
        InitTestInfo(expectedInfo, {
            expectedInfo->kings[white] = CreateBitboard(1, b2);
            expectedInfo->pawns[white] = CreateBitboard(1, f2);

            expectedInfo->kings[black] = CreateBitboard(1, a6);
            expectedInfo->pawns[black] = CreateBitboard(1, d7);
            expectedInfo->knights[black] = CreateBitboard(1, e4);
        });

        state.enPassantSquares = CreateBitboard(1, f3);
    } else {
        InitTestInfo(expectedInfo, {
            expectedInfo->kings[white] = CreateBitboard(1, b4);
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
        expectedInfo->pawns[white] = CreateBitboard(1, f2);

        expectedInfo->kings[black] = CreateBitboard(1, a6);
        expectedInfo->pawns[black] = CreateBitboard(1, d7);
        expectedInfo->knights[black] = CreateBitboard(1, e4);
    });

    GameState_t state = ReadDefaultNextGameState();
    *expectedState = state;
}

// TESTS
static void ShouldCastleKingside() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitAllCastlingLegalInfo(&info);
    InitKingsideCastleExpectedInfo(&expectedInfo, &expectedState);

    Move_t ksCastle;
    InitMove(&ksCastle);
    WriteFromSquare(&ksCastle, LSB(info.kings[white]));
    WriteToSquare(&ksCastle, LSB(white_kingside_castle_sq));
    WriteSpecialFlag(&ksCastle, castle_flag);

    MakeMove(&info, ksCastle, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldCastleQueenside() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitAllCastlingLegalInfo(&info);
    InitQueensideCastleExpectedInfo(&expectedInfo, &expectedState);

    Move_t qsCastle;
    InitMove(&qsCastle);
    WriteFromSquare(&qsCastle, LSB(info.kings[white]));
    WriteToSquare(&qsCastle, LSB(white_queenside_castle_sq));
    WriteSpecialFlag(&qsCastle, castle_flag);

    MakeMove(&info, qsCastle, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldQuietPromote() {
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

    MakeMove(&info, move, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldCapturePromote() {
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

    MakeMove(&info, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldWhiteEnPassant() {
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

    MakeMove(&info, move, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldBlackEnPassant() {
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

    MakeMove(&info, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldMakeNormalQuietMoves() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitNormalPosition(&info);
    InitNormalQuietExpected(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, b2);
    WriteToSquare(&move, c3);

    MakeMove(&info, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
    PrintChessboard(&info); PrintChessboard(&expectedInfo);
}

static void ShouldDoublePushPawns(Color_t color) {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitNormalPosition(&info);
    InitPawnDoublePushExpected(&expectedInfo, &expectedState, color);

    Move_t move;
    InitMove(&move);
    if(color == white) {
        WriteFromSquare(&move, f2);
        WriteToSquare(&move, f4);
    } else {
        WriteFromSquare(&move, d7);
        WriteToSquare(&move, d5);
    }

    MakeMove(&info, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

static void ShouldMakeNormalCaptures() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitNormalPosition(&info);
    InitNormalCaptureExpectedPosition(&expectedInfo, &expectedState);

    Move_t move;
    InitMove(&move);
    WriteFromSquare(&move, e4);
    WriteToSquare(&move, f2);

    MakeMove(&info, move, black);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

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

    ResetGameStateStack();
}

void UnmakeMoveTDDRunner() {

}