#include "game_state_tdd.h"
#include "bitboards.h"
#include "debug.h"

enum {
    some_captured_piece = queen,
    some_halfmove_clock = 16,
    some_white_castle_squares = 0x562a,
    some_white_enpassant_squares = 0x8,
    some_black_castle_squares = 0x56da,
    some_black_enpassant_squares = 0x81
};

// HELPERS
static bool GameStateIsCorrect(GameStack_t* stack, GameState_t* expected) {
    return 
        (ReadCapturedPiece(stack) == expected->capturedPiece) &&
        (ReadHalfmoveClock(stack) == expected->halfmoveClock) &&
        (ReadCastleSquares(stack, white) == expected->castleSquares[white]) &&
        (ReadCastleSquares(stack, black) == expected->castleSquares[black]) &&
        (ReadEnPassant(stack) == expected->enPassantSquares);
}

static GameState_t* GetSomeGamestate(GameStack_t* stack) {
    GameState_t* state = GetEmptyNextGameState(stack);

    state->capturedPiece = some_captured_piece;
    state->halfmoveClock = some_halfmove_clock;
    state->castleSquares[white] = some_white_castle_squares;
    state->enPassantSquares = some_white_enpassant_squares;
    state->castleSquares[black] = some_black_castle_squares;

    return state;
}

// TESTS
static void ShouldAddState() {
    GameStack_t stack;
    InitGameStack(&stack);
    GameState_t* state = GetSomeGamestate(&stack);

    PrintResults(GameStateIsCorrect(&stack, state));
}

static void ShouldGetDefaultState() {
    GameStack_t stack;
    InitGameStack(&stack);
    GameState_t* state = GetSomeGamestate(&stack);

    GameState_t expected = {
        .capturedPiece = none_type,
        .halfmoveClock = state->halfmoveClock + 1,
        .castleSquares = {state->castleSquares[white], state->castleSquares[black]},
        .enPassantSquares = empty_set
    };

    GetDefaultNextGameState(&stack);

    PrintResults(GameStateIsCorrect(&stack, &expected));
}

static void ShouldRevertState() {
    GameStack_t stack;
    InitGameStack(&stack);
    GameState_t* state1 = GetSomeGamestate(&stack);
    GetDefaultNextGameState(&stack);

    RevertState(&stack);

    PrintResults(GameStateIsCorrect(&stack, state1));
}

void GameStateTDDRunner() {
    ShouldAddState();
    ShouldGetDefaultState();
    ShouldRevertState();
}