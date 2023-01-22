#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "debug.h"
#include "legals_tdd.h"
#include "legals.h"
#include "pieces.h"
#include "board_constants.h"
#include "lookup.h"

enum {
    white_expected_unsafe = 0xfefb57be78800000,
    black_expected_unsafe = 0x20c0f5fa9ddfdffe
};

// HELPERS

#define KingPos(boardInfo, color) LSB(info.kings[color])

static bool IsInCheck(BoardInfo_t* boardInfo, Color_t color) {
    if(color == white) {
        return InCheck(boardInfo, WhiteUnsafeSquares(boardInfo), white);
    } else {
        return InCheck(boardInfo, BlackUnsafeSquares(boardInfo), black);
    }
}

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

// 4k3/8/8/8/8/8/8/Rb2Kb1R
static void InitWhiteCastlingBlockedInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, e1);
    info->rooks[white] = CreateBitboard(2, a1,h1);

    info->kings[black] = CreateBitboard(1, e8);
    info->bishops[black] = CreateBitboard(2, b1,f1);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// 7k/2q5/8/5b2/3K4/8/1b6/8
static void InitWhiteSlidingCheckmaskTestInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, d4);

    info->kings[black] = CreateBitboard(1, h8);
    info->bishops[black] = CreateBitboard(2, b2,f5);
    info->queens[black] = CreateBitboard(1, c7);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// 7k/8/2p2pp1/4p3/3K4/8/8/8
static void InitWhitePawnCheckmaskTestInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, d4);

    info->kings[black] = CreateBitboard(1, h8);
    info->pawns[black] = CreateBitboard(4, e5,c6,f6,g6);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// 8/7K/2P5/3k2P1/4P3/2P5/8/8
static void InitBlackPawnCheckmaskTestInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, h7);
    info->pawns[white] = CreateBitboard(4, c3,e4,g5,c6);

    info->kings[black] = CreateBitboard(1, d5);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// 7k/2n5/7q/2K5/4n3/1b1N4/8/8
static void InitWhiteKnightCheckmaskTestInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, c5);
    info->knights[white] = CreateBitboard(1, d3);

    info->kings[black] = CreateBitboard(1, h8);
    info->knights[black] = CreateBitboard(2, e4,c7);
    info->bishops[black] = CreateBitboard(1, b3);
    info->queens[black] = CreateBitboard(1, h6);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// 3r3k/2q5/8/5b2/3K4/8/1b6/8
static void InitDoubleSlidingCheckTestInfo(BoardInfo_t* info) {
    InitWhiteSlidingCheckmaskTestInfo(info);
    info->rooks[black] = CreateBitboard(1, d8);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// TESTS

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

    Bitboard_t expectedWhiteCastling = white_queenside_castle_sq | white_kingside_castle_sq;
    Bitboard_t expectedBlackCastling = black_queenside_castle_sq | black_kingside_castle_sq;

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

static void ShouldntCastleThroughBlockers() {
    BoardInfo_t info;
    InitWhiteCastlingBlockedInfo(&info);

    Bitboard_t expectedWhiteCastling = empty_set;

    bool success = CastlingMoves(&info, WhiteUnsafeSquares(&info), white) == expectedWhiteCastling;

    PrintResults(success);
}

static void ShouldFindInCheck() {
    BoardInfo_t info;
    InitMidgameInfo(&info);

    bool success = !IsInCheck(&info, white);

    PrintResults(success);
}

static void TestSlidingCheckCheckmask() {
    BoardInfo_t info;
    InitWhiteSlidingCheckmaskTestInfo(&info);

    Bitboard_t expected = CreateBitboard(2, b2, c3);

    bool success = DefineCheckmask(&info, white) == expected;

    PrintResults(success);
}

static void TestWhitePawnCheckCheckmask() {
    BoardInfo_t info;
    InitWhitePawnCheckmaskTestInfo(&info);

    Bitboard_t expected = CreateBitboard(1, e5);

    bool success = DefineCheckmask(&info, white) == expected;

    PrintResults(success);
}

static void TestBlackPawnCheckCheckmask() {
    BoardInfo_t info;
    InitBlackPawnCheckmaskTestInfo(&info);

    Bitboard_t expected = CreateBitboard(1, e4);

    bool success = DefineCheckmask(&info, black) == expected;

    PrintResults(success);
}

static void TestKnightCheckCheckmask() {
    BoardInfo_t info;
    InitWhiteKnightCheckmaskTestInfo(&info);

    Bitboard_t expected = CreateBitboard(1, e4);

    bool success = DefineCheckmask(&info, white) == expected;

    PrintResults(success);
}

static void ShouldIdentifySliderDoubleCheck() {
    BoardInfo_t info;
    InitDoubleSlidingCheckTestInfo(&info);

    bool expected = true;

    assert(IsInCheck(&info, white));
    bool actual = IsDoubleCheck(&info, DefineCheckmask(&info, white), white);

    PrintResults(expected == actual);
}

static void ShouldNotIdentifySingleChecksAsDoubleChecks() {
    BoardInfo_t info;
    InitWhiteSlidingCheckmaskTestInfo(&info);

    bool expected = false;

    assert(IsInCheck(&info, white));
    bool actual = IsDoubleCheck(&info, DefineCheckmask(&info, white), white);

    PrintResults(expected == actual);
}

void LegalsTDDRunner() {
    TestWhiteUnsafeSquares();
    TestBlackUnsafeSquares();

    TestKingLegalMoves();

    TestAllLegalCastling();
    ShouldntCastleThroughCheck();
    ShouldntCastleThroughBlockers();

    ShouldFindInCheck();
    
    TestSlidingCheckCheckmask();
    TestWhitePawnCheckCheckmask();
    TestBlackPawnCheckCheckmask();
    TestKnightCheckCheckmask();

    ShouldIdentifySliderDoubleCheck();
    ShouldNotIdentifySingleChecksAsDoubleChecks();
}