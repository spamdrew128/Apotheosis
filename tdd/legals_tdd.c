#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "legals_tdd.h"
#include "legals.h"
#include "pieces.h"
#include "board_constants.h"

enum {
    white_expected_unsafe = 0xfefb57be78800000,
    black_expected_unsafe = 0x20c0f5fa9ddfdffe
};

#define KingPos(boardInfo, color) LSB(info.kings[color])

// r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R
static void InitMidgameInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->pawns[white] = CreateBitboard(7, a2,b2,c2,d5,e4,f2,g5);
    info->knights[white] = CreateBitboard(2, c3,f3);
    info->bishops[white] = CreateBitboard(1, f1);
    info->rooks[white] = CreateBitboard(2, d1,h1);
    info->queens[white] = CreateBitboard(1, h6);
    info->kings[white] = CreateBitboard(1, c1);

    info->pawns[black] = CreateBitboard(8, a7,b7,c6,d6,e5,f7,g6,h7);
    info->knights[black] = CreateBitboard(2, e7,f6);
    info->bishops[black] = CreateBitboard(1, c8);
    info->rooks[black] = CreateBitboard(2, a8,f8);
    info->queens[black] = CreateBitboard(1, e8);
    info->kings[black] = CreateBitboard(1, g8);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// 8/8/8/8/4K3/8/8/k3r3
static void InitKingLegalsTestInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, e4);

    info->rooks[black] = CreateBitboard(1, e1);
    info->kings[black] = CreateBitboard(1, a1);

    UpdateAllPieces(info);
    UpdateEmpty(info);
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
}

// 4k1r1/8/8/7b/8/8/8/R3K2R
static void InitWhiteCastlingIllegalInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, e1);
    info->rooks[white] = CreateBitboard(2, a1,h1);

    info->kings[black] = CreateBitboard(1, e8);
    info->bishops[black] = CreateBitboard(1, h5);
    info->rooks[black] = CreateBitboard(1, g8);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

static void TestWhiteUnsafeSquares() {
    BoardInfo_t info;
    InitMidgameInfo(&info);

    PrintResults(WhiteUnsafeSquares(&info) == white_expected_unsafe);
}

static void TestBlackUnsafeSquares() {
    BoardInfo_t info;
    InitMidgameInfo(&info);

    PrintResults(BlackUnsafeSquares(&info) == black_expected_unsafe);
}

static void TestKingLegalMoves() {
    BoardInfo_t info;
    InitKingLegalsTestInfo(&info);
    Bitboard_t expectedKingLegals = CreateBitboard(6, d3,d4,d5,f3,f4,f5);

    Bitboard_t kingMoves = KingMoveTargets(KingPos(info, white), info.empty);
    Bitboard_t unsafeSquares = WhiteUnsafeSquares(&info);
    PrintResults(KingLegalMoves(kingMoves, unsafeSquares) == expectedKingLegals);
}

static void TestAllLegalCastling() {
    BoardInfo_t info;
    InitAllCastlingLegalInfo(&info);

    Bitboard_t expectedWhiteCastling = white_queenside_castle | white_kingside_castle;
    Bitboard_t expectedBlackCastling = black_queenside_castle | black_kingside_castle;

    bool success = 
        (CastlingMoves(&info, WhiteUnsafeSquares(&info), white) == expectedWhiteCastling) &&
        (CastlingMoves(&info, BlackUnsafeSquares(&info), black) == expectedBlackCastling);

    PrintResults(success);
}

static void ShouldntCastleThroughCheck() {
    BoardInfo_t info;
    InitWhiteCastlingIllegalInfo(&info);

    Bitboard_t expectedWhiteCastling = empty_set;

    bool success = CastlingMoves(&info, WhiteUnsafeSquares(&info), white) == expectedWhiteCastling;

    PrintResults(success);
}

void LegalsTDDRunner() {
    TestWhiteUnsafeSquares();
    TestBlackUnsafeSquares();

    TestKingLegalMoves();

    TestAllLegalCastling();
    ShouldntCastleThroughCheck();
}