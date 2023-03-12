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

    Move_t historyMove = NullMove();
    WriteFromSquare(&historyMove, a2);
    WriteToSquare(&historyMove, a3);

    MoveList_t moveList;
    CompleteMovegen(&moveList, &boardInfo, &gameStack);

    UpdateHistory(&history, &boardInfo, &moveList, historyMove, some_depth);
    PrintResults(HistoryScore(&history, &boardInfo, historyMove) == some_depth*some_depth);
}

void HistoryTDDRunner() {
    ShouldUpdate();
}