#include <stdio.h>

#include "debug.h"

void PrintBitboard(Bitboard_t b) {
    Square_t boardArray[64] = {0};

    while(b) {
        Square_t square = __builtin_ctzll(b);
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

