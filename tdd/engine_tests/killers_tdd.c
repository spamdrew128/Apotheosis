#include "killers_tdd.h"
#include "debug.h"

enum {
    some_move_data = 831,
    some_other_move_data = 7,

    some_ply = 38,
};

static void ShouldInitAllMoves() {
    Killers_t killers;
    InitKillers(&killers);

    bool success = true;
    for(int i = 0; i < KILLERS_LENGTH; i++) {
        for(int j = 0; j < KILLERS_WIDTH; j++) {
            if(!CompareMoves(killers.moves[i][j], NullMove())) {
                success = false;
                break;
            }
        }
    }

    PrintResults(success);
}

static void ShouldShiftKillersWhenNewKillerAdded() {
    Killers_t killers;
    InitKillers(&killers);

    Move_t oldKiller = NullMove();
    oldKiller.data = some_move_data;
    AddKiller(&killers, oldKiller, some_ply);

    Move_t newKiller = NullMove();
    newKiller.data = some_other_move_data;
    AddKiller(&killers, newKiller, some_ply);

    bool success = 
        CompareMoves(GetKiller(&killers, some_ply, 0), newKiller) &&
        CompareMoves(GetKiller(&killers, some_ply, 1), oldKiller);

    PrintResults(success);
}

void KillersTDDRunner() {
    ShouldInitAllMoves();
    ShouldShiftKillersWhenNewKillerAdded();
}