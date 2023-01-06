#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "pieces_tdd.h"
#include "pieces.h"
#include "board_constants.h"
#include "bitboards.h"

enum {
    start_empty = rank_3 | rank_4 | rank_5 | rank_6,
    start_w = rank_2,
    start_b = rank_7,
    start_expected_w_single = rank_3,
    start_expected_b_single = rank_6,
    start_expected_w_double = rank_4,
    start_expected_b_double = rank_5,

    midgame_empty = 0x8A4C13A7EFDBD853,
    midgame_w = 0x4810002700,
    midgame_b = 0xA34C1000000000
};

// HELPERS

// TESTS
static void StartSinglePawnPushesMatch() {
    bool success = 
        (WhiteSinglePushTargets(start_w, start_empty) == start_expected_w_single) &&
        (BlackSinglePushTargets(start_b, start_empty) == start_expected_b_single);
    PrintResults(success);
}

static void StartDoublePawnPushesMatch() {
    bool success = 
        (WhiteDoublePushTargets(start_w, start_empty) == start_expected_w_double) &&
        (BlackDoublePushTargets(start_b, start_empty) == start_expected_b_double);
    PrintResults(success);
}

static void ComplexSinglePawnPushesMatch() {
    Bitboard_t expectedWhite = CreateBitboard(2, a3,b3);
    Bitboard_t expectedBlack = CreateBitboard(3, a6,b6,c5);

    bool success = 
        (WhiteSinglePushTargets(midgame_w, midgame_empty) == expectedWhite) &&
        (BlackSinglePushTargets(midgame_b, midgame_empty) == expectedBlack);
    PrintResults(success);
}

static void ComplexDoublePawnPushesMatch() {
    Bitboard_t expectedWhite = CreateBitboard(2, a4,b4);
    Bitboard_t expectedBlack = CreateBitboard(2, a5,b5);

    bool success = 
        (WhiteDoublePushTargets(midgame_w, midgame_empty) == expectedWhite) &&
        (BlackDoublePushTargets(midgame_b, midgame_empty) == expectedBlack);
    PrintResults(success);
}

static void StartPushablePawnsMatch() {
    bool success =
        (WhitePushablePawns(start_w, start_empty) == start_w) &&
        (WhiteDblPushablePawns(start_w, start_empty) == start_w) &&
        (BlackPushablePawns(start_b, start_empty) == start_b) &&
        (BlackDblPushablePawns(start_b, start_empty) == start_b);
    PrintResults(success);
}

static void ComplexPushablePawnsMatch() {
    Bitboard_t expectedWhite = CreateBitboard(2, a2,b2);
    Bitboard_t expectedDblWhite = CreateBitboard(2, a2,b2);
    Bitboard_t expectedBlack = CreateBitboard(3, a7,b7,c6);
    Bitboard_t expectedDblBlack = CreateBitboard(2, a7,b7);

    bool success =
        (WhitePushablePawns(midgame_w, midgame_empty) == expectedWhite) &&
        (WhiteDblPushablePawns(midgame_w, midgame_empty) == expectedDblWhite) &&
        (BlackPushablePawns(midgame_b, midgame_empty) == expectedBlack) &&
        (BlackDblPushablePawns(midgame_b, midgame_empty) == expectedDblBlack);
    PrintResults(success);
}

void PiecesTDDRunner() {
    StartSinglePawnPushesMatch();
    StartDoublePawnPushesMatch();
    ComplexSinglePawnPushesMatch();
    ComplexDoublePawnPushesMatch();
    StartPushablePawnsMatch();
    ComplexPushablePawnsMatch();
}