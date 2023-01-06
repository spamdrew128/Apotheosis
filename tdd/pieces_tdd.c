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
    start_expected_b_double = fifth_rank
};

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

void PiecesTDDRunner() {
    StartSinglePawnPushesMatch();
    StartDoublePawnPushesMatch();
}