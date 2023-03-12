#ifndef __HISTORY_H__
#define __HISTORY_H__

#include "board_constants.h"
#include "bitboards.h"
#include "movegen.h"
#include "engine_types.h"

typedef struct {
    Move_t moves[MOVELIST_MAX];
    MoveIndex_t quietMaxIndex;
} QuietMovesList_t;

typedef struct {
    MoveScore_t scores[NUM_PIECES][NUM_SQUARES];
    QuietMovesList_t quietMoves;
} History_t;

void InitHistory(History_t* history);

void InitQuietMovesList(QuietMovesList_t* quiets);

void AddQuietMove(QuietMovesList_t* quiets, Move_t move);

void UpdateHistory(History_t* history, BoardInfo_t* boardInfo, QuietMovesList_t* quiets, Depth_t depth);

MoveScore_t HistoryScore(History_t* history, BoardInfo_t* boardInfo, Move_t move);

void AgeHistory(History_t* history);

#endif