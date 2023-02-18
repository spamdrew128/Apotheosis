#ifndef __BOARD_INFO_H__
#define __BOARD_INFO_H__

#include "board_constants.h"

typedef struct {
    Color_t sideToMove;

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

Piece_t PieceOnSquare(BoardInfo_t* boardInfo, Square_t square);

void AddPieceToMailbox(BoardInfo_t* boardInfo, Square_t square, Piece_t piece);

void RemovePieceFromMailbox(BoardInfo_t* boardInfo, Square_t square);

void MovePieceInMailbox(BoardInfo_t* boardInfo, Square_t toSquare, Square_t fromSquare);

Bitboard_t* GetPieceInfoField(BoardInfo_t* boardInfo, Piece_t piece, Color_t color);

Square_t KingSquare(BoardInfo_t* boardInfo, Color_t color);

Bitboard_t AllHvSliders(BoardInfo_t* boardInfo, Color_t color);

Bitboard_t AllD12Sliders(BoardInfo_t* boardInfo, Color_t color);

#endif