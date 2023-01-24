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
    Move_t moves[256];
    int numMoves;
} MoveList_t;

#endif