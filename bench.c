#include <stdio.h>
#include <string.h>

#include "bench.h"
#include "timer.h"
#include "perft_table_entries.h"
#include "chess_search.h"
#include "board_info.h"
#include "game_state.h"
#include "zobrist.h"
#include "FEN.h"

#define LINE_BUFFER 500

bool Bench(int argc, char** argv) {
    if(argc != 2 || strcmp(argv[1], "bench")) {
        return true; // keep running
    }

    FILE* rFP = fopen("wahoo_datagen_1.txt", "r");
    FILE* wFP = fopen("resolved.txt", "w");

    uint64_t writes = 0;
    uint64_t reads = 0;

    char buffer[LINE_BUFFER];
    while(fgets(buffer, LINE_BUFFER, rFP)) {
        reads++;

        BoardInfo_t boardInfo;
        GameStack_t gameStack;
        ZobristStack_t zobristStack;
        InterpretFEN(buffer, &boardInfo, &gameStack, &zobristStack);

        UciSearchInfo_t info;
        UciSearchInfoInit(&info);
        info.forceTime = 20;
        info.overhead = 0;

        int i = 0;
        EvalScore_t upper = 50;
        EvalScore_t lower = -50;
        while(i < LINE_BUFFER) {
            if(buffer[i] == '.') {
                if(buffer[i - 1] == '1') { // win
                    lower = 200;
                    upper = EVAL_MAX;
                } else if(buffer[i + 1] == '0') { // loss
                    lower = -EVAL_MAX;
                    upper = -200;
                }
            }
            i++;
        }

        if(reads % 10000 == 0) {
            printf("%lld reads\n%lld saved\n\n", (long long)reads, (long long) writes);
        }
    }

    printf("%lld reads\n%lld saved\n\n", (long long)reads, (long long) writes);

    fclose(rFP);
    fclose(wFP);
}