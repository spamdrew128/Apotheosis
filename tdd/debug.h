#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__

#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"
#include "board_info.h"
#include "movegen.h"

#define PrintResults(success) \
    if(success) {printf(".");} else {printf("\n%s Failure\n", __func__);}

#define InitTestInfo(infoPtr, ...) \
    do { \
        InitBoardInfo(info); \
        __VA_ARGS__ \
        UpdateAllPieces(info); \
        UpdateEmpty(info); \
        TranslateBitboardsToMailbox(info); \
    } while(0)

// To avoid lookup dependancies
#define SquareToBitset(square) C64(1) << square

void PrintBitboard(Bitboard_t b);

void PrintMailbox(BoardInfo_t *info);

void PrintChessboard(BoardInfo_t* info);

void PrintMoveList(MoveList_t* moveList, BoardInfo_t* info);

Bitboard_t CreateBitboard(int numOccupied, ...);

#endif