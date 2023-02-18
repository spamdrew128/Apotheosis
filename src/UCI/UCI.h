#ifndef __UCI_H__
#define __UCI_H__

#include <stdbool.h>

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"
#include "zobrist.h"

bool UCITranslateMove(Move_t* move, const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack);

void SendNumericalUciCommand(const char* command, int data);

bool InterpretUCIInput(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
);

#endif