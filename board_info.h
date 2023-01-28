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

void UpdateAllPieces(BoardInfo_t* boardInfo);

void UpdateEmpty(BoardInfo_t* boardInfo);

void TranslateBitboardsToMailbox(BoardInfo_t* info);

#define PieceOnSquare(boardInfoPtr, square) \
    boardInfoPtr->mailbox[square]

#endif