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
    QuietList_t quiets;
    InitQuietList(&quiets);
    AddQuietMove(&quiets, move);

    UpdateHistory(&history, &boardInfo, &quiets, some_depth);
    PrintResults(HistoryScore(&history, &boardInfo, move) == 1200);
}

void HistoryTDDRunner() {
    ShouldUpdate();
}