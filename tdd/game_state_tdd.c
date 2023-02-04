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
static bool GameStateIsCorrect(GameStateOld_t* expected) {
    return 
        (ReadCapturedPieceOld() == expected->capturedPiece) &&
        (ReadHalfmoveClockOld() == expected->halfmoveClock) &&
        (ReadCastleSquaresOld(white) == expected->castleSquares[white]) &&
        (ReadCastleSquaresOld(black) == expected->castleSquares[black]) &&
        (ReadEnPassantSquaresOld() == expected->enPassantSquares);
}

static GameStateOld_t* GetSomeGamestate() {
    GameStateOld_t* state = GetEmptyNextGameStateOld();

    state->capturedPiece = some_captured_piece;
    state->halfmoveClock = some_halfmove_clock;
    state->castleSquares[white] = some_white_castle_squares;
    state->enPassantSquares = some_white_enpassant_squares;
    state->castleSquares[black] = some_black_castle_squares;

    return state;
}

// TESTS
static void ShouldAddState() {
    GameStateOld_t* state = GetSomeGamestate();

    PrintResults(GameStateIsCorrect(state));
    ResetGameStateStackOld();
}

static void ShouldGetDefaultState() {
    GameStateOld_t* state = GetSomeGamestate();

    GameStateOld_t expected = {
        .capturedPiece = none_type,
        .halfmoveClock = state->halfmoveClock + 1,
        .castleSquares = {state->castleSquares[white], state->castleSquares[black]},
        .enPassantSquares = empty_set
    };

    GetDefaultNextGameStateOld();

    PrintResults(GameStateIsCorrect(&expected));
    ResetGameStateStackOld();
}

static void ShouldRevertStateOld() {
    GameStateOld_t* state1 = GetSomeGamestate();
    GetDefaultNextGameStateOld(state1);

    RevertStateOld();

    PrintResults(GameStateIsCorrect(state1));
    ResetGameStateStackOld();
}

void GameStateTDDRunner() {
    ShouldAddState();
    ShouldGetDefaultState();
    ShouldRevertStateOld();
}