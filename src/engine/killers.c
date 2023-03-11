#include "assert.h"

#include "killers.h"

void InitKillers(Killers_t* killers) {
    for(int i = 0; i < KILLERS_LENGTH; i++) {
        for(int j = 0; j < KILLERS_WIDTH; j++) {
            InitMove(&(killers->moves[i][j]));
        }
    }
}

void AddKiller(Killers_t* killers, Move_t move, Ply_t ply) {
    // for loop just in case I want to make it wider later
    // the compiler should know how to optimize it out
    for(int i = 0; i < KILLERS_WIDTH - 1; i++) {
        killers->moves[ply][i+1] = killers->moves[ply][i];
    }
    killers->moves[ply][0] = move;
}

Move_t GetKiller(Killers_t* killers, Ply_t ply, int index) {
    assert(index < KILLERS_WIDTH);
    return killers->moves[ply][index];
}