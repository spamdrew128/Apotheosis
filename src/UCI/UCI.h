#ifndef __UCI_H__
#define __UCI_H__

#include <stdbool.h>
#include <stdio.h>

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"
#include "zobrist.h"

typedef struct
{
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
} UciApplicationData_t;


bool UCITranslateMove(Move_t* move, const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack);

bool InterpretUCIInput(UciApplicationData_t* applicationData);

#define SendUciInfoString(formatString, ...) \
do { \
    printf("info "); \
    printf(formatString, __VA_ARGS__); \
    printf("\n"); \
} while(0)

#endif