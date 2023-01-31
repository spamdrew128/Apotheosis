#include "make_and_unmake_tdd.h"
#include "movegen.h"
#include "game_state.h"
#include "board_info.h"
#include "debug.h"

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
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, e1);
    info->rooks[white] = CreateBitboard(2, a1,h1);

    info->kings[black] = CreateBitboard(1, e8);
    info->rooks[black] = CreateBitboard(2, a8,h8);

    UpdateAllPieces(info);
    UpdateEmpty(info);
    TranslateBitboardsToMailbox(info);

    AddStartingGameState();
}

// r3k2r/8/8/8/8/8/8/R4RK1
static void InitKingsideCastleExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitBoardInfo(expectedInfo);
    expectedInfo->kings[white] = CreateBitboard(1, g1);
    expectedInfo->rooks[white] = CreateBitboard(2, a1,f1);

    expectedInfo->kings[black] = CreateBitboard(1, e8);
    expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);

    UpdateAllPieces(expectedInfo);
    UpdateEmpty(expectedInfo);
    TranslateBitboardsToMailbox(expectedInfo);

    GameState_t temp = GetDefaultNextGameState();
    expectedState->halfmoveClock = temp.halfmoveClock;
    expectedState->castleSquares[white] = empty_set;
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = temp.enPassantSquares;
}

// r3k2r/8/8/8/8/8/8/R4RK1
static void InitQueensideCastleExpectedInfo(BoardInfo_t* expectedInfo, GameState_t* expectedState) {
    InitBoardInfo(expectedInfo);
    expectedInfo->kings[white] = CreateBitboard(1, c1);
    expectedInfo->rooks[white] = CreateBitboard(2, d1,h1);

    expectedInfo->kings[black] = CreateBitboard(1, e8);
    expectedInfo->rooks[black] = CreateBitboard(2, a8,h8);

    UpdateAllPieces(expectedInfo);
    UpdateEmpty(expectedInfo);
    TranslateBitboardsToMailbox(expectedInfo);

    GameState_t temp = GetDefaultNextGameState();
    expectedState->halfmoveClock = temp.halfmoveClock;
    expectedState->castleSquares[white] = empty_set;
    expectedState->castleSquares[black] = temp.castleSquares[black];
    expectedState->enPassantSquares = temp.enPassantSquares;
}

static void ShouldCastleKingside() {
    BoardInfo_t info;
    BoardInfo_t expectedInfo;
    GameState_t expectedState;
    InitAllCastlingLegalInfo(&info);
    InitKingsideCastleExpectedInfo(&expectedInfo, &expectedState);

    Move_t ksCastle;
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
    WriteFromSquare(&qsCastle, LSB(info.kings[white]));
    WriteToSquare(&qsCastle, LSB(white_queenside_castle_sq));
    WriteSpecialFlag(&qsCastle, castle_flag);

    MakeMove(&info, qsCastle, white);

    bool infoMatches = CompareInfo(&info, &expectedInfo);
    bool stateMatches = CompareState(&expectedState);

    PrintResults(infoMatches && stateMatches);
}

void MakeAndUnmakeTDDRunner() {
    ShouldCastleKingside();

    ResetGameStateStack();
}