#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game_state.h"

#define CurrentState(stack) stack->gameStates[stack->top]

#define NextState(stack) stack->gameStates[stack->top + 1]

enum {
    stack_empty = -1
};

void InitGameStack(GameStack_t* stack) {
    stack->top = stack_empty;
}

GameState_t* GetEmptyNextGameState(GameStack_t* stack) {
    GameState_t* nextState = &(NextState(stack));

    nextState->capturedPiece = none_type;
    nextState->halfmoveClock = 0;
    nextState->castleSquares[white] = empty_set;
    nextState->castleSquares[black] = empty_set;
    nextState->enPassantSquares = empty_set;
    InitBoardInfo(&nextState->boardInfo);

    stack->top++;
    return nextState;
}

GameState_t* GetDefaultNextGameState(GameStack_t* stack) {
    GameState_t* defaultState = &(NextState(stack));

    defaultState->capturedPiece = none_type;
    defaultState->halfmoveClock = ReadHalfmoveClock(stack) + 1;
    defaultState->castleSquares[white] = ReadCastleSquares(stack, white);
    defaultState->castleSquares[black] = ReadCastleSquares(stack, black);
    defaultState->enPassantSquares = empty_set;

    stack->top++;
    return defaultState;
}

void AddStartingGameState(GameStack_t* stack) {
    GameState_t* gameStartState = GetEmptyNextGameState(stack);

    gameStartState->castleSquares[white] = white_kingside_castle_bb | white_queenside_castle_bb;
    gameStartState->castleSquares[black] = black_kingside_castle_bb | black_queenside_castle_bb;
}

void RevertState(GameStack_t* stack) {
    assert(stack->top >= 0);
    stack->top--;
}

Piece_t ReadCapturedPiece(GameStack_t* stack) {
    return CurrentState(stack).capturedPiece;
}

HalfmoveCount_t ReadHalfmoveClock(GameStack_t* stack) {
    return CurrentState(stack).halfmoveClock;
}

Bitboard_t ReadCastleSquares(GameStack_t* stack, Color_t color) {
    return CurrentState(stack).castleSquares[color];
}

Bitboard_t ReadEnPassant(GameStack_t* stack) {
    return CurrentState(stack).enPassantSquares;
}

BoardInfo_t ReadCurrentBoardInfo(GameStack_t* stack) {
    return CurrentState(stack).boardInfo;
}

GameState_t ReadCurrentGameState(GameStack_t* stack) {
    return CurrentState(stack);
}

GameState_t ReadDefaultNextGameState(GameStack_t* stack) {
    GameState_t* nextState = GetDefaultNextGameState(stack);
    stack->top--;
    return *nextState;
}