#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <stdint.h>
#include "board_constants.h"

typedef uint16_t HalfmoveCount_t;
typedef struct {
    Piece_t capturedPiece;
    HalfmoveCount_t halfmoveClock;
    Bitboard_t enPassantSquares;
    Bitboard_t castleSquares[2];
} GameState_t;

GameState_t* GetEmptyNextGameState();

GameState_t* GetDefaultNextGameState();

void AddStartingGameState();

void RevertState();

Piece_t ReadCapturedPiece();

HalfmoveCount_t ReadHalfmoveClock();

Bitboard_t ReadCastleSquares(Color_t color);

Bitboard_t ReadEnPassantSquares();

GameState_t ReadCurrentGameState();

GameState_t ReadDefaultNextGameState();

void ResetGameStateStack();

#endif