#include <assert.h>

#include "history.h"
#include "util_macros.h"

enum {
    history_cap = 1200
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

    MoveScore_t currentScore = history->scores[piece][toSquare];
    history->scores[piece][toSquare] += MIN(depth * depth, history_cap);
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);
    return history->scores[piece][toSquare];
}

void AgeHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[i][j] /= 2;
        }
    }
}