#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "bitboards.h"

Bitboard_t NortOne (Bitboard_t b) {return  b << 8;}
Bitboard_t NoEaOne (Bitboard_t b) {return (b & not_h_file) << 9;}
Bitboard_t EastOne (Bitboard_t b) {return (b & not_h_file) << 1;}
Bitboard_t SoEaOne (Bitboard_t b) {return (b & not_h_file) >> 7;}
Bitboard_t SoutOne (Bitboard_t b) {return  b >> 8;}
Bitboard_t SoWeOne (Bitboard_t b) {return (b & not_a_file) >> 9;}
Bitboard_t WestOne (Bitboard_t b) {return (b & not_a_file) >> 1;}
Bitboard_t NoWeOne (Bitboard_t b) {return (b & not_a_file) << 7;}

Bitboard_t NortTwo(Bitboard_t b) {return  b << 16;}
Bitboard_t SoutTwo(Bitboard_t b) {return  b >> 16;}

Population_t PopulationCount(Bitboard_t b) {
    Population_t count = 0;
    while(b) {
        ResetLSB(&b);
        count++;
    }
    return count;
}

Square_t LSB(Bitboard_t b) { // I hate macros and you can't force me to use them
    assert(b);
    return __builtin_ctzll(b);
}

Bitboard_t IsolateLSB(Bitboard_t b) {
    return b & -b;
}

void ResetLSB(Bitboard_t* b) {
    *b &= *b - 1;
}

void SetBits(Bitboard_t* b, Bitboard_t mask) {
    *b |= mask;
}

void ResetBits(Bitboard_t* b, Bitboard_t mask) {
    *b &= ~mask;
}

void ToggleBits(Bitboard_t* b, Bitboard_t mask) {
    *b ^= mask;
}

