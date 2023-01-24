#include "game_state.h"

typedef struct {
    GameState_t gameStates[MOVELIST_MAX];
    int top;
} Stack_t;

static Stack_t stack = {
    .top = 0
};