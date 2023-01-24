#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <stdint.h>
#include "board_constants.h"

typedef uint16_t HalfmoveCount_t;
typedef struct {
    HalfmoveCount_t halfmoveClock;
    Bitboard_t castleSquares[2];
    Bitboard_t enPassantSquares[2];
} GameState_t;

#endif