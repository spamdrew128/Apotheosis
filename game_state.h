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

typedef struct {
    GameState_t gameStates[GAMESTATES_MAX];
    int top;
} GameStateStack_t;

void InitGameStateStack(GameStateStack_t* stack);

GameState_t* GetEmptyNextGameState(GameStateStack_t* stack);

GameState_t* GetDefaultNextGameState(GameStateStack_t* stack);

void AddStartingGameState(GameStateStack_t* stack);

void RevertState(GameStateStack_t* stack);

Piece_t ReadCapturedPiece(GameStateStack_t* stack);

HalfmoveCount_t ReadHalfmoveClock(GameStateStack_t* stack);

Bitboard_t ReadCastleSquares(GameStateStack_t* stack, Color_t color);

Bitboard_t ReadEnPassantSquares(GameStateStack_t* stack);

GameState_t ReadDefaultNextGameState(GameStateStack_t* stack);

void ResetGameStateStack(GameStateStack_t* stack);

#endif