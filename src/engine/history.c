#include <assert.h>

#include "history.h"
#include "util_macros.h"

enum {
    bonus_max = 2500,
};

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[white][i][j] = 0;
            history->scores[black][i][j] = 0;
        }
    }
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, Move_t move, Depth_t depth) {
    const Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    const Square_t toSquare = ReadToSquare(move);
    const Color_t color = boardInfo->colorToMove;
    history->scores[color][piece][toSquare] += MIN(depth * depth, bonus_max);

    if(history->scores[color][piece][toSquare] > history_max) {
        history->scores[color][piece][toSquare] = history_max;
    }
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    const Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    const Square_t toSquare = ReadToSquare(move);
    const Color_t color = boardInfo->colorToMove;

    return history->scores[color][piece][toSquare];
}