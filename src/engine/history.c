#include <assert.h>

#include "history.h"
#include "util_macros.h"

enum {
    bonus_max = 2500,
};

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[i][j] = 0;
        }
    }
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, Move_t move, Depth_t depth) {
    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);

    history->scores[piece][toSquare] += MIN(depth * depth, bonus_max);

    if(history->scores[piece][toSquare] > history_max) {
        history->scores[piece][toSquare] = history_max;
    }
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);
    return history->scores[piece][toSquare];
}