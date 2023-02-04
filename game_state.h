#ifndef __GAME_STATE_OLD_H__
#define __GAME_STATE_OLD_H__

#include <stdint.h>
#include "board_constants.h"

typedef uint16_t HalfmoveCount_t;
typedef struct {
    Piece_t capturedPiece;
    HalfmoveCount_t halfmoveClock;
    Bitboard_t enPassantSquares;
    Bitboard_t castleSquares[2];
} GameStateOld_t;

GameStateOld_t* GetEmptyNextGameStateOld();

GameStateOld_t* GetDefaultNextGameStateOld();

void AddStartingGameStateOld();

void RevertStateOld();

Piece_t ReadCapturedPieceOld();

HalfmoveCount_t ReadHalfmoveClockOld();

Bitboard_t ReadCastleSquaresOld(Color_t color);

Bitboard_t ReadEnPassantSquaresOld();

GameStateOld_t ReadCurrentGameStateOld();

GameStateOld_t ReadDefaultNextGameStateOld();

void ResetGameStateStackOld();

#endif