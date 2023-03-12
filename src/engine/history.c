#include <assert.h>

#include "history.h"
#include "util_macros.h"

enum {
    delta_minimum = 1200,
    delta_multiplier = 16,
    bonus_scaler = 32768,
    history_cap = 30000,
};

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[i][j] = 0;
        }
    }
}

void InitQuietMovesList(QuietMovesList_t* quiets) {
    quiets->quietMaxIndex = -1;
}

void AddQuietMove(QuietMovesList_t* quiets, Move_t move) {
    quiets->quietMaxIndex += 1;
    quiets->moves[quiets->quietMaxIndex] = move;
}

static void UpdateHistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move, Depth_t depth, bool isBestMove) {
    MoveScore_t delta = MIN(delta_multiplier*(depth * depth), delta_minimum);
    MoveScore_t bonus = isBestMove ? delta : -delta;

    Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    Square_t toSquare = ReadToSquare(move);

    MoveScore_t scaledBonus = bonus - (history->scores[piece][toSquare] * delta / bonus_scaler);
    history->scores[piece][toSquare] += scaledBonus;
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, QuietMovesList_t* quiets, Depth_t depth) {
    Move_t bestMove = quiets->moves[quiets->quietMaxIndex];
    UpdateHistoryScore(history, boardInfo, bestMove, depth, true);

    for(MoveIndex_t i = 0; i < quiets->quietMaxIndex; i++) {
        UpdateHistoryScore(history, boardInfo, quiets->moves[i], depth, false);
    } 
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