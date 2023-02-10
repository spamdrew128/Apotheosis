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
    InitCastleRightsToZero(&nextState->castleRights);
    nextState->enPassantSquares = empty_set;
    InitBoardInfo(&nextState->boardInfo);

    stack->top++;
    return nextState;
}

GameState_t* GetDefaultNextGameState(GameStack_t* stack) {
    GameState_t* defaultState = &(NextState(stack));

    defaultState->capturedPiece = none_type;
    defaultState->halfmoveClock = ReadHalfmoveClock(stack) + 1;
    defaultState->castleRights = ReadCastleRights(stack);
    defaultState->enPassantSquares = empty_set;

    stack->top++;
    return defaultState;
}

void AddStartingGameState(GameStack_t* stack) {
    GameState_t* gameStartState = GetEmptyNextGameState(stack);

    SetKingsideCastleRights(&gameStartState->castleRights, white);
    SetQueensideCastleRights(&gameStartState->castleRights, white);
    SetKingsideCastleRights(&gameStartState->castleRights, black);
    SetQueensideCastleRights(&gameStartState->castleRights, black);
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

CastleRights_t ReadCastleRights(GameStack_t* stack) {
    return CurrentState(stack).castleRights;
}

Bitboard_t ReadEnPassantSquares(GameStack_t* stack) {
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