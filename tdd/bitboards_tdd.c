#include <stdio.h>
#include <stdbool.h>

#include "debug_macros.h"
#include "bitboards_tdd.h"
#include "bitboards.h"

static void PrintBitboard() {

}

static void CanCountPopulation() {
    Bitboard_t b = C64(0b110110010111010);
    Population_t expected = 9;
    PrintResults(PopulationCount(b) == expected);
}

static void CanIdentifyLSB() {
    Bitboard_t b = C64(0b111000);
    Square_t expected = d1;
    PrintResults(LSB(b) == expected);
}

void BitboardsTDDRunner() {
    CanCountPopulation();
    CanIdentifyLSB();
}
