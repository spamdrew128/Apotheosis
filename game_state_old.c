#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game_state_old.h"

typedef struct {
    GameStateOld_t gameStates[GAMESTATES_MAX];
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

GameStateOld_t* GetEmptyNextGameStateOld() {
    GameStateOld_t* nextState = &(NextState(stack));

    nextState->capturedPiece = none_type;
    nextState->halfmoveClock = 0;
    nextState->castleSquares[white] = empty_set;
    nextState->castleSquares[black] = empty_set;
    nextState->enPassantSquares = empty_set;

    stack.top++;
    return nextState;
}

GameStateOld_t* GetDefaultNextGameStateOld() {
    GameStateOld_t* defaultState = &(NextState(stack));

    defaultState->capturedPiece = none_type;
    defaultState->halfmoveClock = ReadHalfmoveClockOld() + 1;
    defaultState->castleSquares[white] = ReadCastleSquaresOld(white);
    defaultState->castleSquares[black] = ReadCastleSquaresOld(black);
    defaultState->enPassantSquares = empty_set;

    stack.top++;
    return defaultState;
}

void AddStartingGameStateOld() {
    GameStateOld_t* gameStartState = GetEmptyNextGameStateOld();

    gameStartState->capturedPiece = none_type;
    gameStartState->halfmoveClock = 0;
    gameStartState->castleSquares[white] = white_kingside_castle_bb | white_queenside_castle_bb;
    gameStartState->castleSquares[black] = black_kingside_castle_bb | black_queenside_castle_bb;
    gameStartState->enPassantSquares = empty_set;
}

void RevertStateOld() {
    assert(stack.top >= 0);
    stack.top--;
}

Piece_t ReadCapturedPieceOld() {
    return CurrentState(stack).capturedPiece;
}

HalfmoveCount_t ReadHalfmoveClockOld() {
    return CurrentState(stack).halfmoveClock;
}

Bitboard_t ReadCastleSquaresOld(Color_t color) {
    return CurrentState(stack).castleSquares[color];
}

Bitboard_t ReadEnPassantSquaresOld() {
    return CurrentState(stack).enPassantSquares;
}

GameStateOld_t ReadCurrentGameStateOld() {
    return CurrentState(stack);
}

GameStateOld_t ReadDefaultNextGameStateOld() {
    GameStateOld_t* nextState = GetDefaultNextGameStateOld();
    stack.top--;
    return *nextState;
}

void ResetGameStateStackOld() {
    stack.top = stack_empty;
}