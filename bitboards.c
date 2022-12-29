#include <stdio.h>
#include <assert.h>

#include "bitboards.h"

Bitboard_t NortOne (Bitboard_t b) {return  b << 8;}
Bitboard_t NoEaOne (Bitboard_t b) {return (b & not_h_file) << 9;}
Bitboard_t EastOne (Bitboard_t b) {return (b & not_h_file) << 1;}
Bitboard_t SoEaOne (Bitboard_t b) {return (b & not_h_file) >> 7;}
Bitboard_t SoutOne (Bitboard_t b) {return  b >> 8;}
Bitboard_t SoWeOne (Bitboard_t b) {return (b & not_a_file) >> 9;}
Bitboard_t WestOne (Bitboard_t b) {return (b & not_a_file) >> 1;}
Bitboard_t NoWeOne (Bitboard_t b) {return (b & not_a_file) << 7;}

Population_t PopulationCount(Bitboard_t b) {
    Population_t count = 0;
    while(b) {
        b &= b - 1;
        count++;
    }
    return count;
}

Square_t Lsb(Bitboard_t b) {
    assert(b);
    return __builtin_ctzll(b);
}

