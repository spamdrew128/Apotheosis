#include <assert.h>

#include "history.h"

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[white][i][j] = 0;
            history->scores[black][i][j] = 0;
        }
    }
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, Move_t move, Depth_t depth) {
    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);
    history->scores[boardInfo->colorToMove][piece][toSquare] += depth * depth;
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);
    return history->scores[boardInfo->colorToMove][piece][toSquare];
}