#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__

#include <stdio.h>
#include <assert.h>

#include "board_constants.h"
#include "bitboards.h"
#include "board_info.h"
#include "movegen.h"
#include "game_state.h"

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

void PrintMoveList(MoveEntryList_t* moveList, BoardInfo_t* info);

void PrintMove(Move_t move, bool hasNewline);

Bitboard_t CreateBitboard(int numOccupied, ...);

void AddGameStateToStack(GameState_t stateToAdd, GameStack_t* stack);

bool CompareInfo(BoardInfo_t* info, BoardInfo_t* expectedInfo);

bool CompareState(GameState_t* expectedState, GameStack_t* stack);

bool BoardIsValid(BoardInfo_t *info, GameStack_t* gameStack);

bool CompareMoves(Move_t m1, Move_t m2);

#endif