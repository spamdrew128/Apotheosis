#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game_state.h"

typedef struct {
    GameState_t* gameStates[GAMESTATES_MAX];
    int top;
} Stack_t;

enum {
    stack_empty = -1
};

#define CurrentState(stack) stack.gameStates[stack.top]

#define NextState(stack) stack.gameStates[++(stack.top)]

#define FreeTopOfStack(stack) free(stack.gameStates[(stack.top)--])

static Stack_t stack = {
    .top = stack_empty,
};

GameState_t* GetNewGameState() {
    GameState_t* newState = malloc(sizeof(*newState));
    assert(newState);
    return newState;
}

GameState_t* GetDefaultGameState() {
    GameState_t* defaultState = GetNewGameState();

    defaultState->halfmoveClock = ReadHalfmoveClock() + 1;
    defaultState->castleSquares[white] = ReadCastleSquares(white);
    defaultState->castleSquares[black] = ReadCastleSquares(black);
    defaultState->enPassantSquares[white] = empty_set;
    defaultState->enPassantSquares[black] = empty_set;

    return defaultState;
}

void AddState(GameState_t* newState) {
    NextState(stack) = newState;
}

void RevertState() {
    assert(stack.top >= 0);
    FreeTopOfStack(stack);
}

HalfmoveCount_t ReadHalfmoveClock() {
    return CurrentState(stack)->halfmoveClock;
}

Bitboard_t ReadCastleSquares(Color_t color) {
    return CurrentState(stack)->castleSquares[color];
}

Bitboard_t ReadEnPassantSquares(Color_t color) {
    return CurrentState(stack)->enPassantSquares[color];
}

void TeardownGameStateStack() {
    while(stack.top > stack_empty) {
        RevertState();
    }
}