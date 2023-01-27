#ifndef __BOARD_INFO_H__
#define __BOARD_INFO_H__

#include "board_constants.h"

typedef struct {
    Bitboard_t allPieces[2];
    Bitboard_t empty;

    Bitboard_t pawns[2];
    Bitboard_t knights[2];
    Bitboard_t bishops[2];
    Bitboard_t rooks[2];
    Bitboard_t queens[2];
    Bitboard_t kings[2];

    Piece_t mailbox[NUM_SQUARES];
} BoardInfo_t;

void InitBoardInfo(BoardInfo_t* info);

#define UpdateAllPieces(boardInfoPtr) \
    for(int i = 0; i < 2; i++) { \
        boardInfoPtr->allPieces[i] = \
            boardInfoPtr->pawns[i] | \
            boardInfoPtr->knights[i] | \
            boardInfoPtr->bishops[i] | \
            boardInfoPtr->rooks[i] | \
            boardInfoPtr->queens[i] | \
            boardInfoPtr->kings[i]; \
    }

#define UpdateEmpty(boardInfoPtr) \
    boardInfoPtr->empty = ~((boardInfoPtr)->allPieces[white] | (boardInfoPtr)->allPieces[black])

void TranslateBitboardsToMailbox(BoardInfo_t* info);

#define PieceOnSquare(boardInfoPtr, square) \
    boardInfoPtr->mailbox[square]

#endif