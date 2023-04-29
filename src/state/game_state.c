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
    nextState->castleSquares[white] = EMPTY_SET;
    nextState->castleSquares[black] = EMPTY_SET;
    nextState->enPassantSquare = EMPTY_SET;
    nextState->canEastEP = false;
    nextState->canWestEP = false;
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
    defaultState->enPassantSquare = EMPTY_SET;
    defaultState->canEastEP = false;
    defaultState->canWestEP = false;

    stack->top++;
    return defaultState;
}

void AddStartingGameState(GameStack_t* stack) {
    GameState_t* gameStartState = GetEmptyNextGameState(stack);

    gameStartState->castleSquares[white] = WHITE_KINGSIDE_CASTLE_BB | WHITE_QUEENSIDE_CASTLE_BB;
    gameStartState->castleSquares[black] = BLACK_KINGSIDE_CASTLE_BB | BLACK_QUEENSIDE_CASTLE_BB;
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
    return CurrentState(stack).enPassantSquare;
}

bool CanWestEnPassant(GameStack_t* stack) {
    return CurrentState(stack).canWestEP;
}

bool CanEastEnPassant(GameStack_t* stack) {
    return CurrentState(stack).canEastEP;
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