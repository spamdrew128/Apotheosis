#include "debug.h"
#include "history.h"
#include "FEN.h"

enum {
    some_depth = 24
};

void ShouldUpdateHistory() {
    History_t history;
    InitHistory(&history);
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    InterpretFEN(START_FEN, &boardInfo, &gameStack, &zobristStack);

    Move_t move1 = NullMove();
    WriteFromSquare(&move1, a2);
    WriteToSquare(&move1, a3);

    Move_t bestMove = NullMove();
    WriteFromSquare(&bestMove, a2);
    WriteToSquare(&bestMove, a4);

    QuietMovesList_t quiets;
    InitQuietMovesList(&quiets);
    AddQuietMove(&quiets, move1);
    AddQuietMove(&quiets, bestMove);

    UpdateHistory(&history, &boardInfo, &quiets, some_depth);
    PrintResults(
        HistoryScore(&history, &boardInfo, move1) < 0 &&
        HistoryScore(&history, &boardInfo, bestMove) > 0
    );
}

void HistoryTDDRunner() {
    ShouldUpdateHistory();
}