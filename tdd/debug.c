#include <stdio.h>

#include "debug.h"

static void FillBoardArray(char boardArray[], Bitboard_t b, char fillChar) {
    while(b) {
        Square_t square = __builtin_ctzll(b);
        boardArray[square] = fillChar;
        b &= b - 1;
    }
}

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
            Square_t square = boardArray[i*8 + j];
            if(square) {
                printf("%d ", boardArray[i*8 + j]);
            } else {
                printf(". ");
            }
        }    
        printf("\n");
    }
}

void PrintChessboard(BoardInfo_t* info) {
    char boardArray[64] = {0};
    FillBoardArray(boardArray, ~C64(0), '.');

    FillBoardArray(boardArray, info->rooks[white], 'R');
    FillBoardArray(boardArray, info->knights[white], 'N');
    FillBoardArray(boardArray, info->bishops[white], 'B');
    FillBoardArray(boardArray, info->queens[white], 'Q');
    FillBoardArray(boardArray, info->kings[white], 'K');
    FillBoardArray(boardArray, info->pawns[white], 'P');

    FillBoardArray(boardArray, info->rooks[black], 'r');
    FillBoardArray(boardArray, info->knights[black], 'n');
    FillBoardArray(boardArray, info->bishops[black], 'b');
    FillBoardArray(boardArray, info->queens[black], 'q');
    FillBoardArray(boardArray, info->kings[black], 'k');
    FillBoardArray(boardArray, info->pawns[black], 'p');

    printf("\n");
    for(int i = 7; i >= 0; i--) {
        for(int j = 0; j < 8; j++) {
            printf("%c ", boardArray[i*8 + j]);
        }    
        printf("\n");
    }
}