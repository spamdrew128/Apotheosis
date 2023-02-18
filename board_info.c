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
    info->colorToMove = white;

    TranslateBitboardsToMailbox(info);
}

static void AddPieceBBToMailbox(BoardInfo_t* info, Piece_t piece, Bitboard_t pieceBitboard) {
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
    AddPieceBBToMailbox(info, none_type, full_set);
    AddPieceBBToMailbox(info, king, info->kings[white] | info->kings[black]);
    AddPieceBBToMailbox(info, pawn, info->pawns[white] | info->pawns[black]);
    AddPieceBBToMailbox(info, knight, info->knights[white] | info->knights[black]);
    AddPieceBBToMailbox(info, bishop, info->bishops[white] | info->bishops[black]);
    AddPieceBBToMailbox(info, rook, info->rooks[white] | info->rooks[black]);
    AddPieceBBToMailbox(info, queen, info->queens[white] | info->queens[black]);
}

Piece_t PieceOnSquare(BoardInfo_t* boardInfo, Square_t square) {
    return boardInfo->mailbox[square];
}

void AddPieceToMailbox(BoardInfo_t* boardInfo, Square_t square, Piece_t piece) {
    boardInfo->mailbox[square] = piece;
}

void RemovePieceFromMailbox(BoardInfo_t* boardInfo, Square_t square) {
    boardInfo->mailbox[square] = none_type;
}

void MovePieceInMailbox(BoardInfo_t* boardInfo, Square_t toSquare, Square_t fromSquare) {
    boardInfo->mailbox[toSquare] = boardInfo->mailbox[fromSquare];
    RemovePieceFromMailbox(boardInfo, fromSquare);
}

Bitboard_t* GetPieceInfoField(BoardInfo_t* boardInfo, Piece_t piece, Color_t color) {
    switch(piece)
    {
        case knight:
            return &(boardInfo->knights[color]);
        case bishop:
            return &(boardInfo->bishops[color]);
        case rook:
            return &(boardInfo->rooks[color]);
        case queen:
            return &(boardInfo->queens[color]);
            case pawn:
            return &(boardInfo->pawns[color]);
        default:
            return &(boardInfo->kings[color]);
    }
}

Square_t KingSquare(BoardInfo_t* boardInfo, Color_t color) {
    return LSB(boardInfo->kings[color]);
}

Bitboard_t AllHvSliders(BoardInfo_t* boardInfo, Color_t color) {
    return boardInfo->rooks[color] | boardInfo->queens[color];
}

Bitboard_t AllD12Sliders(BoardInfo_t* boardInfo, Color_t color) {
    return boardInfo->bishops[color] | boardInfo->queens[color];
}