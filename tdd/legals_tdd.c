#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "legals_tdd.h"
#include "legals.h"
#include "board_constants.h"

enum {
    white_expected_unsafe = 0xfefb57be78800000,
    black_expected_unsafe = 0x20c0f5fa9ddfdffe
};

// r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R
static void InitMidgameInfo(BoardInfo_t* info) {
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

void LegalsTDDRunner() {
    TestWhiteUnsafeSquares();
    TestBlackUnsafeSquares();
}