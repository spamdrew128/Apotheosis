#include "game_state_tdd.h"
#include "bitboards.h"
#include "debug.h"

enum {
    some_color_to_move = black,
    some_halfmove_clock = 16,
    some_white_castle_squares = 0x562a,
    some_white_enpassant_squares = 0x8,
    some_black_castle_squares = 0x56da,
    some_black_enpassant_squares = 0x81
};

// HELPERS
static bool GameStateIsCorrect(GameState_t expected) {
    return 
        (ReadColorToMove() == expected.colorToMove) &&
        (ReadHalfmoveClock() == expected.halfmoveClock) &&
        (ReadCastleSquares(white) == expected.castleSquares[white]) &&
        (ReadCastleSquares(black) == expected.castleSquares[black]) &&
        (ReadEnPassantSquares(white) == expected.enPassantSquares[white]) &&
        (ReadEnPassantSquares(black) == expected.enPassantSquares[black]);
}

static GameState_t GetSomeGamestate() {
    GameState_t state = GetNewGameState();

    state.colorToMove = some_color_to_move;
    state.halfmoveClock = some_halfmove_clock;
    state.castleSquares[white] = some_white_castle_squares;
    state.enPassantSquares[white] = some_white_enpassant_squares;
    state.castleSquares[black] = some_black_castle_squares;
    state.enPassantSquares[black] = some_black_enpassant_squares;

    return state;
}

// TESTS
static void ShouldAddState() {
    GameState_t state = GetSomeGamestate();

    AddState(state);

    PrintResults(GameStateIsCorrect(state));
    ResetGameStateStack();
}

static void ShouldGetDefaultState() {
    GameState_t state = GetSomeGamestate();
    AddState(state);

    GameState_t nextState = GetDefaultNextGameState();
    AddState(nextState);

    GameState_t expected = {
        .colorToMove = !some_color_to_move,
        .halfmoveClock = state.halfmoveClock + 1,
        .castleSquares = {state.castleSquares[white], state.castleSquares[black]},
        .enPassantSquares = {empty_set, empty_set}
    };

    PrintResults(GameStateIsCorrect(expected));
    ResetGameStateStack();
}

static void ShouldRevertState() {
    GameState_t state1 = GetSomeGamestate();
    GameState_t state2 = GetDefaultNextGameState(state1);
    
    AddState(state1);
    AddState(state2);

    RevertState();

    PrintResults(GameStateIsCorrect(state1));
    ResetGameStateStack();
}

void GameStateTDDRunner() {
    ShouldAddState();
    ShouldGetDefaultState();
    ShouldRevertState();
    stack.top = 5;
}