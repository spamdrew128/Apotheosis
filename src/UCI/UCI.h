#ifndef __UCI_H__
#define __UCI_H__

#include <stdbool.h>
#include <stdio.h>

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"
#include "zobrist.h"
#include "PV_table.h"
#include "chess_search.h"

typedef struct
{
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    UciSearchInfo_t uciSearchInfo;
} UciApplicationData_t;

void UciApplicationDataInit(UciApplicationData_t* data);

bool UCITranslateMove(Move_t* move, const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack);

bool InterpretUCIInput(UciApplicationData_t* applicationData);

void InterpretUCIString(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    const char* _input
);

void SendPvInfo(PvTable_t* pvTable);

#define SendUciInfoString(formatString, pvTablePtr, ...) \
do { \
    printf("info "); \
    printf(formatString, __VA_ARGS__); \
    SendPvInfo((pvTablePtr)); \
    printf("\n"); \
} while(0)

#endif