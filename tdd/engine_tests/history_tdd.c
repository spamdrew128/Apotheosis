#include "debug.h"
#include "history.h"
#include "FEN.h"

enum {
    some_depth = 24
};

void ShouldUpdate() {
    History_t history;
    InitHistory(&history);
    BoardInfo_t boardInfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    InterpretFEN(START_FEN, &boardInfo, &gameStack, &zobristStack);

    Move_t move = NullMove();
    WriteFromSquare(&move, a2);
    WriteToSquare(&move, a3);

    UpdateHistory(&history, &boardInfo, move, some_depth);
    PrintResults(HistoryScore(&history, &boardInfo, move) == some_depth*some_depth);
}

void HistoryTDDRunner() {
    ShouldUpdate();
}