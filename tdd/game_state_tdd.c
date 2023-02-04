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
static bool GameStateIsCorrect(GameState_t* expected) {
    return 
        (ReadCapturedPiece() == expected->capturedPiece) &&
        (ReadHalfmoveClock() == expected->halfmoveClock) &&
        (ReadCastleSquares(white) == expected->castleSquares[white]) &&
        (ReadCastleSquares(black) == expected->castleSquares[black]) &&
        (ReadEnPassantSquares() == expected->enPassantSquares);
}

static GameState_t* GetSomeGamestate() {
    GameState_t* state = GetEmptyNextGameState();

    state->capturedPiece = some_captured_piece;
    state->halfmoveClock = some_halfmove_clock;
    state->castleSquares[white] = some_white_castle_squares;
    state->enPassantSquares = some_white_enpassant_squares;
    state->castleSquares[black] = some_black_castle_squares;

    return state;
}

// TESTS
static void ShouldAddState() {
    GameState_t* state = GetSomeGamestate();

    PrintResults(GameStateIsCorrect(state));
    ResetGameStateStack();
}

static void ShouldGetDefaultState() {
    GameState_t* state = GetSomeGamestate();

    GameState_t expected = {
        .capturedPiece = none_type,
        .halfmoveClock = state->halfmoveClock + 1,
        .castleSquares = {state->castleSquares[white], state->castleSquares[black]},
        .enPassantSquares = empty_set
    };

    GetDefaultNextGameState();

    PrintResults(GameStateIsCorrect(&expected));
    ResetGameStateStack();
}

static void ShouldRevertState() {
    GameState_t* state1 = GetSomeGamestate();
    GetDefaultNextGameState(state1);

    RevertState();

    PrintResults(GameStateIsCorrect(state1));
    ResetGameStateStack();
}

void GameStateTDDRunner() {
    ShouldAddState();
    ShouldGetDefaultState();
    ShouldRevertState();
}