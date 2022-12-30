#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "bitboards_tdd.h"
#include "bitboards.h"

enum {
    NineBitBitboard = C64(0b110110010111010),
    d1_LSB = C64(0b111000)
};

static void CanCountPopulation(Bitboard_t b, Population_t expected) {
    PrintResults(PopulationCount(b) == expected);
}

static void CanIdentifyLSB(Bitboard_t b, Square_t expected) {
    PrintResults(LSB(b) == expected);
}

void BitboardsTDDRunner() {
    BoardInfo_t info;
    InitBoardInfo(&info);

    CanCountPopulation(NineBitBitboard, 9);
    CanIdentifyLSB(d1_LSB, d1);
}
