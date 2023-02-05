#include "perft_table.h"
#include "recursive_testing.h"
#include "debug.h"
#include "game_state.h"
#include "movegen.h"
#include "make_and_unmake.h"

static GameStack_t stack;

static void TestSetup() {
    InitGameStack(&stack);
}

static void PERFT(BoardInfo_t* boardInfo, int depth, PerftCount_t* count, Color_t color) {
    MoveList_t moveList;
    CompleteMovegen(&moveList, boardInfo, &stack, color);

    if(depth > 1) {
        for(int i = 0; i <= moveList.maxIndex; i++) {
            Move_t move = moveList.moves[i];
            MakeMove(boardInfo, &stack, move, color);

            PERFT(boardInfo, depth-1, count, !color);

            UnmakeMove(boardInfo, &stack, move, color);
        }
    } else {
        *count += moveList.maxIndex + 1;
        return;
    }
}

void RunAllPerftTests(bool shouldRun) {
    PerftTestContainer_t table[NUM_ENTRYS] = {
        PERFT_TEST_TABLE(EXPAND_AS_TEST_CONTAINER)
    };

    for(int i = 0; i < NUM_ENTRYS; i++) {
        FEN_t fen = table[i].fen;
        for(int j = 0; j < MAX_DEPTH; j++) {
            TestSetup();
            BoardInfo_t info;
            PerftCount_t count = 0;
            Color_t color = InterpretFEN(fen, &info, &stack);

            PerftCount_t expectedCounts = table[i].expectedCounts[j];
            if(expectedCounts) {
                int depth = j + 1;
                PERFT(&info, depth, &count, color);
                
                if(count == expectedCounts) {
                    printf(".");
                } else {
                    printf("\nFailure at FEN %s and depth %d\n", fen, depth);
                }
            }

        }

    }
    
}