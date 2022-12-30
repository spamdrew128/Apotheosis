#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "bitboards.h"
#include "lookup.h"

void InitBoardInfo(BoardInfo_t* info) {
    for(int i = 0; i < 2; i++) {
        info->allPieces[i] = C64(0);
        info->pawns[i] = C64(0);
        info->knights[i] = C64(0);
        info->bishops[i] = C64(0);
        info->rooks[i] = C64(0);
        info->queens[i] = C64(0);
        info->kings[i] = C64(0);
    }
}

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

inline Square_t LSB(Bitboard_t b) { // I hate macros and you can't force me to use them
    assert(b);
    return __builtin_ctzll(b);
}


