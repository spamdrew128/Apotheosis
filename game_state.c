#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game_state.h"

typedef struct {
    GameState_t* gameStates[MOVELIST_MAX];
    int top;
    bool isInitialized;
} Stack_t;

enum {
    stack_empty = -1,
    unused_game_state = -2
};

#define CurrentState(stack) stack.gameStates[stack.top]

#define NextState(stack) stack.gameStates[++(stack.top)]

static Stack_t stack = {
    .isInitialized = false
};

void InitStack() {
    stack.top = stack_empty;
    for(int i = 0; i < MOVELIST_MAX; i++) {
        GameState_t* state = AllocateEmptyGameState();
        assert(state);

        state->halfmoveClock = unused_game_state;
        stack.gameStates[i] = state;
    }

    stack.isInitialized = true;
}

GameState_t* AllocateEmptyGameState() {
    GameState_t* newState = malloc(sizeof(*newState));
    assert(newState != NULL);
    return newState;
}

void AddState(GameState_t* newState) {
    NextState(stack) = newState;
}

void RevertState() {
    assert(stack.top > 0);
    stack.top--;
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

void TeardownStack() {
    assert(stack.isInitialized);

    for(int i = 0; i < MOVELIST_MAX; i++) {
        free(stack.gameStates[i]);
    }

    stack.top = stack_empty;
    stack.isInitialized = false;
}