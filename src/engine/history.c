#include <assert.h>

#include "history.h"

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[i][j] = 0;
        }
    }
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, MoveList_t* moveList, Move_t cutoffMove, Depth_t depth) {
    for(int i = moveList->maxCapturesIndex + 1; i <= moveList->maxIndex; i++) {
        Move_t move = moveList->moves[i].move;
        Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
        Square_t toSquare = ReadToSquare(move);

        if(CompareMoves(move, cutoffMove)) {
            history->scores[piece][toSquare] += depth * depth;
        } else {
            history->scores[piece][toSquare] -= depth * depth;
        }
    }
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);
    return history->scores[piece][toSquare];
}