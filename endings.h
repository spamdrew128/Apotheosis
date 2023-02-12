#ifndef __ENDINGS_H__
#define __ENDINGS_H__

#include <stdint.h>

#include "board_constants.h"
#include "board_info.h"
#include "game_state.h"
#include "movegen.h"
#include "zobrist.h"

typedef uint8_t GameEndStatus_t;
enum {
    ongoing,
    checkmate,
    draw
};

GameEndStatus_t CurrentGameEndStatus(
    BoardInfo_t* boardInfo,
    GameState_t* gameState,
    ZobristStack_t* zobristStack,
    ZobristHash_t positionHash,
    int moveListMaxIndex,
    Color_t colorToMove
);

#endif