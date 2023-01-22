#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "bitboards.h"
#include "lookup.h"

void InitBoardInfo(BoardInfo_t* info) {
    for(int i = 0; i < 2; i++) {
        info->allPieces[i] = empty_set;
        info->pawns[i] = empty_set;
        info->knights[i] = empty_set;
        info->bishops[i] = empty_set;
        info->rooks[i] = empty_set;
        info->queens[i] = empty_set;
        info->kings[i] = empty_set;
    }

    info->empty = empty_set;
    info->castleSquares[white] = white_kingside_castle_sq | white_queenside_castle_sq;
    info->castleSquares[black] = black_kingside_castle_sq | black_queenside_castle_sq;
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
        ResetLSB(b);
        count++;
    }
    return count;
}

inline Square_t LSB(Bitboard_t b) { // I hate macros and you can't force me to use them
    assert(b);
    return __builtin_ctzll(b);
}


