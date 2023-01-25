#include "game_state_tdd.h"
#include "bitboards.h"
#include "debug.h"

enum {
    some_halfmove_clock = 16,
    some_white_castle_squares = 0x562a,
    some_white_enpassant_squares = 0x8,
    some_black_castle_squares = 0x56da,
    some_black_enpassant_squares = 0x81
};

// HELPERS
static bool GameStateIsCorrect(GameState_t* expected) {
    return 
        (ReadHalfmoveClock() == expected->halfmoveClock) &&
        (ReadCastleSquares(white) == expected->castleSquares[white]) &&
        (ReadCastleSquares(black) == expected->castleSquares[black]) &&
        (ReadEnPassantSquares(white) == expected->enPassantSquares[white]) &&
        (ReadEnPassantSquares(black) == expected->enPassantSquares[black]);
}

static GameState_t* GetSomeGamestate() {
    GameState_t* state = AllocateEmptyGameState();
    state->halfmoveClock = some_halfmove_clock;
    state->castleSquares[white] = some_white_castle_squares;
    state->enPassantSquares[white] = some_white_enpassant_squares;
    state->castleSquares[black] = some_black_castle_squares;
    state->enPassantSquares[black] = some_black_enpassant_squares;

    return state;
}

// TESTS
static void ShouldAddState() {
    InitStack();
    GameState_t* state = GetSomeGamestate();

    AddState(state);

    PrintResults(GameStateIsCorrect(state));
    TeardownStack();
}

static void ShouldRevertState() {
    InitStack();
    TeardownStack();
}

void GameStateTDDRunner() {
    ShouldAddState();
    ShouldRevertState();
}