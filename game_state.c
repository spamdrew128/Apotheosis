#include <assert.h>
#include <stdlib.h>

#include "game_state.h"

typedef struct {
    GameState_t* gameStates[MOVELIST_MAX];
    int top;
} Stack_t;

#define CurrentState(stack) stack.gameStates[stack.top]

#define NextState(stack) stack.gameStates[++(stack.top)]

static Stack_t stack = {
    .top = 0
};

void AddState(GameState_t* newState) {
    NextState(stack) = newState;
}

void RevertState() {
    assert(stack.top);
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
    for(int i = 0; i < stack.top; i++) {
        free(stack.gameStates[i]);
    }
    stack.top = 0;
}