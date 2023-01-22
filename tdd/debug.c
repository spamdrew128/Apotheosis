#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

static void FillBoardArray(char boardArray[], Bitboard_t b, char fillChar) {
    while(b) {
        Square_t square = __builtin_ctzll(b);
        boardArray[square] = fillChar;
        ResetLSB(b);
    }
}

void PrintBitboard(Bitboard_t b) {
    Square_t boardArray[64] = {0};

    while(b) {
        Square_t square = __builtin_ctzll(b);
        boardArray[square] = 1;
        ResetLSB(b);
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
    FillBoardArray(boardArray, full_set, '.');

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

Bitboard_t CreateBitboard(int numOccupied, ...) {
    va_list valist;
    va_start(valist, numOccupied);

    Bitboard_t bitboard = empty_set;
    for (int i = 0; i < numOccupied; i++) {
        SetBits(bitboard, C64(1) << va_arg(valist, Square_t));
    } 

    return bitboard;
}