#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "pieces_tdd.h"
#include "pieces.h"
#include "board_constants.h"
#include "bitboards.h"

enum {
    start_empty = third_rank | fourth_rank | fifth_rank | sixth_rank,
    start_w = second_rank,
    start_b = seventh_rank,
    start_expected_w_single = third_rank,
    start_expected_b_single = sixth_rank,
    start_expected_w_double = fourth_rank,
    start_expected_b_double = fifth_rank,

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

void PiecesTDDRunner() {
    StartSinglePawnPushesMatch();
    StartDoublePawnPushesMatch();
    ComplexSinglePawnPushesMatch();
    ComplexDoublePawnPushesMatch();
}