#include <stdio.h>
#include <stdbool.h>

#include "debug_macros.h"
#include "bitboards_tdd.h"
#include "bitboards.h"

enum {
    NineBitBitboard = C64(0b110110010111010),
    d1_LSB = C64(0b111000)
};

static void PrintBitboard(Bitboard_t b) {
    Square_t boardArray[64] = {0};

    while(b) {
        Square_t square = LSB(b);
        boardArray[square] = 1;
        b &= b - 1;
    }

    printf("\n");
    for(int i = 7; i >= 0; i--) {
        for(int j = 0; j < 8; j++) {
            printf("%d ", boardArray[i*8 + j]);
        }
        printf("\n");
    }
}

static void CanCountPopulation(Bitboard_t b, Population_t expected) {
    PrintResults(PopulationCount(b) == expected);
}

static void CanIdentifyLSB(Bitboard_t b, Square_t expected) {
    PrintResults(LSB(b) == expected);
}

void BitboardsTDDRunner() {
    CanCountPopulation(NineBitBitboard, 9);
    CanIdentifyLSB(d1_LSB, d1);
}
