#include <assert.h>

#include "history.h"

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_SQUARES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[i][j] = 0;
        }
    }
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, Move_t move, Depth_t depth) {
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);
    history->scores[fromSquare][toSquare] += depth * depth;
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);
    return history->scores[fromSquare][toSquare];
}