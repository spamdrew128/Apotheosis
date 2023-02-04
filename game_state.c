#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game_state.h"

typedef struct {
    GameState_t gameStates[GAMESTATES_MAX];
    int top;
} Stack_t;

enum {
    stack_empty = -1
};

static Stack_t stack = {
    .top = stack_empty,
};

#define CurrentState(stack) stack.gameStates[stack.top]

#define NextState(stack) stack.gameStates[stack.top + 1]

GameState_t* GetEmptyNextGameState() {
    GameState_t* nextState = &(NextState(stack));

    nextState->capturedPiece = none_type;
    nextState->halfmoveClock = 0;
    nextState->castleSquares[white] = empty_set;
    nextState->castleSquares[black] = empty_set;
    nextState->enPassantSquares = empty_set;

    stack.top++;
    return nextState;
}

GameState_t* GetDefaultNextGameState() {
    GameState_t* defaultState = &(NextState(stack));

    defaultState->capturedPiece = ReadCapturedPiece();
    defaultState->halfmoveClock = ReadHalfmoveClock() + 1;
    defaultState->castleSquares[white] = ReadCastleSquares(white);
    defaultState->castleSquares[black] = ReadCastleSquares(black);
    defaultState->enPassantSquares = empty_set;

    stack.top++;
    return defaultState;
}

void AddStartingGameState() {
    GameState_t* gameStartState = GetEmptyNextGameState();

    gameStartState->capturedPiece = none_type;
    gameStartState->halfmoveClock = 0;
    gameStartState->castleSquares[white] = white_kingside_castle_sq | white_queenside_castle_sq;
    gameStartState->castleSquares[black] = black_kingside_castle_sq | black_queenside_castle_sq;
    gameStartState->enPassantSquares = empty_set;
}

void RevertState() {
    assert(stack.top >= 0);
    stack.top--;
}

Piece_t ReadCapturedPiece() {
    return CurrentState(stack).capturedPiece;
}

HalfmoveCount_t ReadHalfmoveClock() {
    return CurrentState(stack).halfmoveClock;
}

Bitboard_t ReadCastleSquares(Color_t color) {
    return CurrentState(stack).castleSquares[color];
}

Bitboard_t ReadEnPassantSquares() {
    return CurrentState(stack).enPassantSquares;
}

GameState_t ReadCurrentGameState() {
    return CurrentState(stack);
}

GameState_t ReadDefaultNextGameState() {
    GameState_t* nextState = GetDefaultNextGameState();
    stack.top--;
    return *nextState;
}

void ResetGameStateStack() {
    stack.top = stack_empty;
}