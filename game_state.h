#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <stdint.h>
#include "board_constants.h"
#include "board_info.h"
#include "castling.h"

typedef uint16_t HalfmoveCount_t;
typedef struct {
    Piece_t capturedPiece;
    HalfmoveCount_t halfmoveClock;
    Bitboard_t enPassantSquares;
    CastleRights_t castleRights;
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

CastleRights_t ReadCastleRights(GameStack_t* stack);

Bitboard_t ReadEnPassantSquares(GameStack_t* stack);

BoardInfo_t ReadCurrentBoardInfo(GameStack_t* stack);

GameState_t ReadCurrentGameState(GameStack_t* stack);

GameState_t ReadDefaultNextGameState(GameStack_t* stack);

#endif