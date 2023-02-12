#ifndef _UCI_H_
#define _UCI_H_

#include "move.h"
#include "board_info.h"
#include "board_constants.h"
#include "game_state.h"

Move_t UCITranslateMove(const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack);

#endif