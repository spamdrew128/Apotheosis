#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__

#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"

#define PrintResults(success) \
    if(success) {printf(".");} else {printf("\n%s Failure\n", __func__);}

void PrintBitboard(Bitboard_t b);

void PrintChessboard(BoardInfo_t* info);

#endif