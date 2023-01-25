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

    for(int i = 0; i < NUM_SQUARES; i++) {
        info->mailbox[i] = none_type;
    }
}

static void AddPieceToMailbox(BoardInfo_t* info, Piece_t piece, Bitboard_t pieceBitboard) {
    while(pieceBitboard) {
        info->mailbox[LSB(pieceBitboard)] = piece;
        ResetLSB(pieceBitboard);
    }
}

void TranslateBitboardsToMailbox(BoardInfo_t* info) {
    AddPieceToMailbox(info, king, info->kings[white] | info->kings[black]);
    AddPieceToMailbox(info, pawn, info->pawns[white] | info->pawns[black]);
    AddPieceToMailbox(info, knight, info->knights[white] | info->knights[black]);
    AddPieceToMailbox(info, bishop, info->bishops[white] | info->bishops[black]);
    AddPieceToMailbox(info, rook, info->rooks[white] | info->rooks[black]);
    AddPieceToMailbox(info, queen, info->queens[white] | info->queens[black]);
}