#include "board_control.h"
#include "pieces.h"
#include "lookup.h"

static Bitboard_t QueenAttacks(Square_t square, Bitboard_t empty) {
    return GetRookAttackSet(square, empty) | GetBishopAttackSet(square, empty);
}

static Bitboard_t PieceAttackSets(BoardInfo_t* boardInfo, Color_t color) {
    const Bitboard_t empty = boardInfo->empty;

    Bitboard_t attackedSquares = empty_set;

    Bitboard_t knights = boardInfo->knights[color];
    Bitboard_t bishops = boardInfo->bishops[color];
    Bitboard_t rooks = boardInfo->rooks[color];
    Bitboard_t queens = boardInfo->queens[color];

    while(knights) {
        Square_t sq = LSB(knights);
        attackedSquares |= GetKnightAttackSet(sq);
        ResetLSB(&knights);
    }
    while(bishops) {
        Square_t sq = LSB(bishops);
        attackedSquares |= GetBishopAttackSet(sq, empty);
        ResetLSB(&bishops);
    }
    while(rooks) {
        Square_t sq = LSB(rooks);
        attackedSquares |= GetRookAttackSet(sq, empty);
        ResetLSB(&rooks);
    }
    while(queens) {
        Square_t sq = LSB(queens);
        attackedSquares |= QueenAttacks(sq, empty);
        ResetLSB(&queens);
    }

    return attackedSquares;
}

void BoardControl(
    BoardInfo_t* boardInfo,
    const Bucket_t wBucket,
    const Bucket_t bBucket,
    Score_t* score
)
{
    const Bitboard_t wPieceAttacks = PieceAttackSets(boardInfo, white);
    const Bitboard_t bPieceAttacks = PieceAttackSets(boardInfo, black);

    const Bitboard_t wPawnAttacks = 
        NoEaOne(boardInfo->pawns[white]) | 
        NoWeOne(boardInfo->pawns[white]);

    const Bitboard_t bPawnAttacks = 
        SoEaOne(boardInfo->pawns[black]) |
        SoWeOne(boardInfo->pawns[black]);

    Bitboard_t allWhiteAttacks = wPieceAttacks | wPawnAttacks;
    Bitboard_t allBlackAttacks = bPieceAttacks | bPawnAttacks;

    Bitboard_t contestedSquares = allWhiteAttacks & allBlackAttacks;
}