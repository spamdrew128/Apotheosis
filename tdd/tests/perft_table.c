#include <assert.h>

#include "perft_table.h"
#include "recursive_testing.h"
#include "debug.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "zobrist.h"
#include "move_ordering.h"
#include "killers.h"
#include "history.h"

static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static void TestSetup() {
    InitGameStack(&gameStack);
}

static void PERFT(BoardInfo_t* boardInfo, int depth, PerftCount_t* count) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &gameStack);

    MovePicker_t movePicker;
    Killers_t dummyKillers;
    History_t dummyHistory;
    InitKillers(&dummyKillers);
    InitHistory(&dummyHistory);
    InitAllMovePicker(&movePicker, &moveList, boardInfo, NullMove(), &dummyKillers, &dummyHistory, 0);

    if(depth > 1) {
        for(int i = 0; i <= moveList.maxIndex; i++) {
            Move_t move = PickMove(&movePicker);
            MakeMove(boardInfo, &gameStack, move);
            assert(BoardIsValid(boardInfo, &gameStack));

            PERFT(boardInfo, depth-1, count);

            UnmakeMove(boardInfo, &gameStack);
            assert(BoardIsValid(boardInfo, &gameStack));
        }
    } else {
        *count += moveList.maxIndex + 1;
        return;
    }
}

void RunAllPerftTests(bool shouldRun) {
    if(!shouldRun) {
        return;
    }

    PerftTestContainer_t table[NUM_PERFT_ENTRIES] = {
        PERFT_TEST_TABLE(EXPAND_AS_TEST_CONTAINER)
    };

    int testsRun = 0;
    for(int i = 0; i < NUM_PERFT_ENTRIES; i++) {
        FEN_t fen = table[i].fen;
        for(int j = 0; j < MAX_DEPTH; j++) {
            TestSetup();
            BoardInfo_t info;
            PerftCount_t count = 0;
            InterpretFEN(fen, &info, &gameStack, &zobristStack);

            PerftCount_t expectedCounts = table[i].expectedCounts[j];
            if(expectedCounts) {
                int depth = j + 1;
                PERFT(&info, depth, &count);
                
                if(count == expectedCounts) {
                    printf(".");
                } else {
                    printf("\nFailure at FEN %s and depth %d\n", fen, depth);
                    printf("Expected %lld and got %lld\n", expectedCounts, count);
                }
            }
        }
        testsRun++;
    } 

    printf("\nRan %d tests!", testsRun);
}