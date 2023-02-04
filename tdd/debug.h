#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__

#include <stdio.h>

#include "board_constants.h"
#include "bitboards.h"
#include "board_info.h"
#include "movegen.h"
#include "game_state_old.h"

#define PrintResults(success) \
    if(success) {printf(".");} else {printf("\n%s Failure\n", __func__);}

#define InitTestInfo(infoPtr, ...) \
    do { \
        InitBoardInfo(infoPtr); \
        __VA_ARGS__ \
        UpdateAllPieces(infoPtr); \
        UpdateEmpty(infoPtr); \
        TranslateBitboardsToMailbox(infoPtr); \
    } while(0)

// To avoid lookup dependancies
#define SquareToBitset(square) C64(1) << square

void PrintBitboard(Bitboard_t b);

void PrintMailbox(BoardInfo_t *info);

void PrintChessboard(BoardInfo_t* info);

void PrintMoveList(MoveList_t* moveList, BoardInfo_t* info);

void PrintMove(Move_t move);

Bitboard_t CreateBitboard(int numOccupied, ...);

void AddGameStateToStack(GameStateOld_t stateToAdd);

bool CompareInfo(BoardInfo_t* info, BoardInfo_t* expectedInfo);

bool CompareState(GameStateOld_t* expectedState);

#endif