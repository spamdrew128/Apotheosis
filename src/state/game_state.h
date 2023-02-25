#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <stdint.h>
#include "board_constants.h"
#include "board_info.h"

typedef uint16_t HalfmoveCount_t;
typedef struct {
    Piece_t capturedPiece;
    bool canEastEP;
    bool canWestEP;
    HalfmoveCount_t halfmoveClock;
    Bitboard_t enPassantSquare;
    Bitboard_t castleSquares[2];
    BoardInfo_t boardInfo;
} GameState_t;

typedef struct {
    GameState_t gameStates[GAMESTATES_MAX];
    int top;
} GameStack_t;

void InitGameStack(GameStack_t* stack);

GameState_t* GetEmptyNextGameState(GameStack_t* stack);

GameState_t* GetDefaultNextGameState(GameStack_t* stack);

void AddStartingGameState(GameStack_t* stack);

void RevertState(GameStack_t* stack);

Piece_t ReadCapturedPiece(GameStack_t* stack);

HalfmoveCount_t ReadHalfmoveClock(GameStack_t* stack);

Bitboard_t ReadCastleSquares(GameStack_t* stack, Color_t color);

Bitboard_t ReadEnPassant(GameStack_t* stack);

bool CanWestEnPassant(GameStack_t* stack);

bool CanEastEnPassant(GameStack_t* stack);

BoardInfo_t ReadCurrentBoardInfo(GameStack_t* stack);

GameState_t ReadCurrentGameState(GameStack_t* stack);

GameState_t ReadDefaultNextGameState(GameStack_t* stack);

#endif