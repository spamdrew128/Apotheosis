#ifndef __UCI_H__
#define __UCI_H__

#include <stdbool.h>

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"

bool UCITranslateMove(Move_t* move, const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack);

bool InterpretUCIInput();

#endif