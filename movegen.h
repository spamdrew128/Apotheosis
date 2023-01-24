#ifndef __MOVEGEN_H__
#define __MOVEGEN_H__

#include "board_constants.h"

typedef struct {
    Piece_t piece;
    Bitboard_t toSquare;
    Bitboard_t fromSquare;
    Piece_t captureTargetType;
    Piece_t promotionType;
} Move_t;

typedef struct {
    Move_t moves[MOVELIST_MAX];
    int numMoves;
} MoveList_t;

void CapturesMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color);

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color);

#define CurrentMove(moveListPtr) moveListPtr->moves[moveListPtr->numMoves]

#endif