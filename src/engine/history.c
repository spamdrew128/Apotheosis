#include <assert.h>
#include <stdint.h>
#include <math.h>

#include "history.h"
#include "util_macros.h"

enum {
    BONUS_MAX = 1200,
    HISTORY_MAX = 32768,
};

typedef int32_t Bonus_t;

void InitHistory(History_t* history) {
    for(int i = 0; i < NUM_PIECES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            history->scores[white][i][j] = 0;
            history->scores[black][i][j] = 0;
        }
    }
}

void InitQuietList(QuietList_t* quietList) {
    quietList->maxIndex = -1;
}

void AddQuietMove(QuietList_t* quietList, Move_t move) {
    quietList->quiets[++quietList->maxIndex] = move;
}

static void UpdateHistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move, Bonus_t bonus) {
    Bonus_t scaledBonus = bonus - HistoryScore(history, boardInfo, move) * abs(bonus) / HISTORY_MAX;
    
    const Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    const Square_t toSquare = ReadToSquare(move);
    const Color_t color = boardInfo->colorToMove;

    history->scores[color][piece][toSquare] += scaledBonus;
}

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, QuietList_t* quietList, Depth_t depth) {
    Bonus_t bonus = MIN((16 * (depth * depth)), BONUS_MAX);

    UpdateHistoryScore(history, boardInfo, quietList->quiets[quietList->maxIndex], bonus); // only the cutoff move gets a positive bonus
    for(int i = 0; i < quietList->maxIndex; i++) {
        const Move_t move = quietList->quiets[i];
        UpdateHistoryScore(history, boardInfo, move, -bonus);
    }
}

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move) {
    const Piece_t piece = PieceOnSquare(boardInfo, ReadFromSquare(move));
    const Square_t toSquare = ReadToSquare(move);
    const Color_t color = boardInfo->colorToMove;

    return history->scores[color][piece][toSquare];
}