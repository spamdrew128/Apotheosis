#include "game_state_tdd.h"
#include "bitboards.h"
#include "debug.h"

enum {
    some_captured_piece = queen,
    some_halfmove_clock = 16,
    some_white_enpassant_squares = 0x8,
    some_black_enpassant_squares = 0x81,
    some_castle_rights_data = 0b1011
};

// HELPERS
static bool GameStateIsCorrect(GameStack_t* stack, GameState_t* expected) {
    return 
        (ReadCapturedPiece(stack) == expected->capturedPiece) &&
        (ReadHalfmoveClock(stack) == expected->halfmoveClock) &&
        CompareCastlingRights(ReadCastleRights(stack), expected->castleRights) && 
        (ReadEnPassantSquares(stack) == expected->enPassantSquares);
}

static GameState_t* GetSomeGamestate(GameStack_t* stack) {
    GameState_t* state = GetEmptyNextGameState(stack);

    state->capturedPiece = some_captured_piece;
    state->halfmoveClock = some_halfmove_clock;
    state->enPassantSquares = some_white_enpassant_squares;
    state->castleRights.data = some_castle_rights_data;

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
        .castleRights = state->castleRights,
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