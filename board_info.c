#include "board_info.h"
#include "bitboards.h"

void InitBoardInfo(BoardInfo_t* info) {
    for(int i = 0; i < 2; i++) {
        info->allPieces[i] = empty_set;
        info->pawns[i] = empty_set;
        info->knights[i] = empty_set;
        info->bishops[i] = empty_set;
        info->rooks[i] = empty_set;
        info->queens[i] = empty_set;
        info->kings[i] = empty_set;
    }

    info->empty = empty_set;

    TranslateBitboardsToMailbox(info);
}

static void AddPieceToMailbox(BoardInfo_t* info, Piece_t piece, Bitboard_t pieceBitboard) {
    while(pieceBitboard) {
        info->mailbox[LSB(pieceBitboard)] = piece;
        ResetLSB(&pieceBitboard);
    }
}

void UpdateAllPieces(BoardInfo_t* boardInfo) {
    for(int i = 0; i < 2; i++) {
        boardInfo->allPieces[i] =
            boardInfo->pawns[i] |
            boardInfo->knights[i] |
            boardInfo->bishops[i] |
            boardInfo->rooks[i] |
            boardInfo->queens[i] |
            boardInfo->kings[i];
    }
}

void UpdateEmpty(BoardInfo_t* boardInfo) {
    boardInfo->empty = ~(boardInfo->allPieces[white] | boardInfo->allPieces[black]);
}

void TranslateBitboardsToMailbox(BoardInfo_t* info) {
    AddPieceToMailbox(info, none_type, full_set);
    AddPieceToMailbox(info, king, info->kings[white] | info->kings[black]);
    AddPieceToMailbox(info, pawn, info->pawns[white] | info->pawns[black]);
    AddPieceToMailbox(info, knight, info->knights[white] | info->knights[black]);
    AddPieceToMailbox(info, bishop, info->bishops[white] | info->bishops[black]);
    AddPieceToMailbox(info, rook, info->rooks[white] | info->rooks[black]);
    AddPieceToMailbox(info, queen, info->queens[white] | info->queens[black]);
}

Piece_t PieceOnSquare(BoardInfo_t* boardInfo, Square_t square) {
    return boardInfo->mailbox[square];
}