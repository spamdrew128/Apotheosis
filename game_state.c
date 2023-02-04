#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game_state.h"

#define CurrentState(stack) stack->gameStates[stack->top]

#define NextState(stack) stack->gameStates[stack->top + 1]

enum {
    stack_empty = -1
};

void InitGameStateStack(GameStateStack_t* stack) {
    stack->top = stack_empty;
}

GameState_t* GetEmptyNextGameState(GameStateStack_t* stack) {
    GameState_t* nextState = &(NextState(stack));

    nextState->capturedPiece = none_type;
    nextState->halfmoveClock = 0;
    nextState->castleSquares[white] = empty_set;
    nextState->castleSquares[black] = empty_set;
    nextState->enPassantSquares = empty_set;

    stack->top++;
    return nextState;
}

GameState_t* GetDefaultNextGameState(GameStateStack_t* stack) {
    GameState_t* defaultState = &(NextState(stack));

    defaultState->capturedPiece = none_type;
    defaultState->halfmoveClock = ReadHalfmoveClock(stack) + 1;
    defaultState->castleSquares[white] = ReadCastleSquares(stack, white);
    defaultState->castleSquares[black] = ReadCastleSquares(stack, black);
    defaultState->enPassantSquares = empty_set;

    stack->top++;
    return defaultState;
}

void AddStartingGameState(GameStateStack_t* stack) {
    GameState_t* gameStartState = GetEmptyNextGameState(stack);

    gameStartState->castleSquares[white] = white_kingside_castle_bb | white_queenside_castle_bb;
    gameStartState->castleSquares[black] = black_kingside_castle_bb | black_queenside_castle_bb;
}

void RevertState(GameStateStack_t* stack) {
    assert(stack->top >= 0);
    stack->top--;
}

Piece_t ReadCapturedPiece(GameStateStack_t* stack) {
    return CurrentState(stack).capturedPiece;
}

HalfmoveCount_t ReadHalfmoveClock(GameStateStack_t* stack) {
    return CurrentState(stack).halfmoveClock;
}

Bitboard_t ReadCastleSquares(GameStateStack_t* stack, Color_t color) {
    return CurrentState(stack).castleSquares[color];
}

Bitboard_t ReadEnPassantSquares(GameStateStack_t* stack) {
    return CurrentState(stack).enPassantSquares;
}

GameState_t ReadCurrentGameState(GameStateStack_t* stack) {
    return CurrentState(stack);
}

GameState_t ReadDefaultNextGameState(GameStateStack_t* stack) {
    GameState_t* nextState = GetDefaultNextGameState(stack);
    stack->top--;
    return *nextState;
}

void ResetGameStateStack(GameStateStack_t* stack) {
    stack->top = stack_empty;
}