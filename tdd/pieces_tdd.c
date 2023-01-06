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
    start_w_all = rank_1 | rank_2,
    start_b_all = rank_7 | rank_8,
    start_expected_w_single = rank_3,
    start_expected_b_single = rank_6,
    start_expected_w_double = rank_4,
    start_expected_b_double = rank_5,

    midgame_empty = 0x8A4C13A7EFDBD853,
    midgame_w = 0x4810002700,
    midgame_b = 0xA34C1000000000,

    other_midgame_w = 0x3001CE00,
    other_midgame_b = 0xE3001C00000000,
    other_midgame_w_all = 0x1003005CEDD,
    other_midgame_b_all = 0xBFE3003C00000000
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

static void MidgameSinglePawnPushesMatch() {
    Bitboard_t expectedWhite = CreateBitboard(2, a3,b3);
    Bitboard_t expectedBlack = CreateBitboard(3, a6,b6,c5);

    bool success = 
        (WhiteSinglePushTargets(midgame_w, midgame_empty) == expectedWhite) &&
        (BlackSinglePushTargets(midgame_b, midgame_empty) == expectedBlack);
    PrintResults(success);
}

static void MidgameDoublePawnPushesMatch() {
    Bitboard_t expectedWhite = CreateBitboard(2, a4,b4);
    Bitboard_t expectedBlack = CreateBitboard(2, a5,b5);

    bool success = 
        (WhiteDoublePushTargets(midgame_w, midgame_empty) == expectedWhite) &&
        (BlackDoublePushTargets(midgame_b, midgame_empty) == expectedBlack);
    PrintResults(success);
}

static void StartCaptureTargets() {
    bool success = 
        (WhiteEastCaptureTargets(start_w, start_b_all) == C64(0)) &&
        (BlackEastCaptureTargets(start_b, start_w_all) == C64(0)) &&
        (WhiteWestCaptureTargets(start_w, start_b_all) == C64(0)) &&
        (BlackWestCaptureTargets(start_b, start_w_all) == C64(0));

    PrintResults(success);
}

static void OtherMidgameCaptureTargets() {
    Bitboard_t expectedWhiteEast = CreateBitboard(1, f5);
    Bitboard_t expectedBlackEast = CreateBitboard(2, e4,f4);
    Bitboard_t expectedWhiteWest = CreateBitboard(2, d5,e5);
    Bitboard_t expectedBlackWest = CreateBitboard(1, a6);

    bool success = 
        (WhiteEastCaptureTargets(other_midgame_w, other_midgame_b_all) == expectedWhiteEast) &&
        (BlackEastCaptureTargets(other_midgame_b, other_midgame_w_all) == expectedBlackEast) &&
        (WhiteWestCaptureTargets(other_midgame_w, other_midgame_b_all) == expectedWhiteWest) &&
        (BlackWestCaptureTargets(other_midgame_b, other_midgame_w_all) == expectedBlackWest);

    PrintResults(success);
}

void PiecesTDDRunner() {
    StartSinglePawnPushesMatch();
    StartDoublePawnPushesMatch();
    MidgameSinglePawnPushesMatch();
    MidgameDoublePawnPushesMatch();

    StartCaptureTargets();
    OtherMidgameCaptureTargets();
}