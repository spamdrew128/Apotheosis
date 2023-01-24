#include "game_state_tdd.h"
#include "bitboards.h"
#include "debug.h"

enum {
    SomeHalfmoveClock = 16,
    SomeCastleSquares = 0x562a,
    SomeEnPassantSquares = 0x8,
    OtherCastleSquares = 0x56da,
    OtherEnPassantSquares = 0x81
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

// TESTS
static void ShouldAddState() {
    GameState_t state = {
        .halfmoveClock = SomeHalfmoveClock,
        .castleSquares = { SomeCastleSquares, OtherCastleSquares },
        .enPassantSquares = { SomeEnPassantSquares, OtherEnPassantSquares }
    };
    AddState(&state);

    PrintResults(GameStateIsCorrect(&state));
    TeardownStack();
}

static void ShouldRevertState() {

}

void GameStateTDDRunner() {
    ShouldAddState();
    ShouldRevertState();
}