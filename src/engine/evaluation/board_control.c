#include "board_control.h"
#include "pieces.h"
#include "lookup.h"
#include "eval_helpers.h"
#include "util_macros.h"

static Score_t boardControlBonus[NUM_PST_BUCKETS][NUM_SQUARES] = BOARD_CONTROL_BONUSES;

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

static void SerializeAttacks(
    Bitboard_t attacks,
    ControlValue_t controlValue,
    ControlValue_t controlSums[NUM_SQUARES]
)
{
    while(attacks) {
        Square_t sq = LSB(attacks);
        controlSums[sq] += controlValue;
        ResetLSB(&attacks);
    }
}

static void HvSliderControl(
    BoardInfo_t* boardInfo,
    Bitboard_t wPieces,
    Bitboard_t bPieces,
    Bitboard_t empty,
    const Bitboard_t contested,
    ControlValue_t controlValue,
    ControlValue_t controlSums[NUM_SQUARES]
)
{
    while(wPieces) {
        Square_t sq = LSB(wPieces);
        
        ResetLSB(&wPieces);
    }
    while(bPieces) {
        Square_t sq = LSB(bPieces);
        controlSums[sq] -= controlValue;
        ResetLSB(&bPieces);
    }
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

    Bitboard_t wPawnAttacksEast = NoEaOne(boardInfo->pawns[white]);
    Bitboard_t wPawnAttacksWest = NoWeOne(boardInfo->pawns[white]);

    Bitboard_t bPawnAttacksEast = SoEaOne(boardInfo->pawns[black]);
    Bitboard_t bPawnAttacksWest= SoWeOne(boardInfo->pawns[black]);

    const Bitboard_t allWhiteAttacks = wPieceAttacks | wPawnAttacksEast | wPawnAttacksWest;
    const Bitboard_t allBlackAttacks = bPieceAttacks | bPawnAttacksEast | bPawnAttacksWest;

    Bitboard_t contestedSquares = allWhiteAttacks & allBlackAttacks;

    Bitboard_t wUncontested = allWhiteAttacks & ~contestedSquares;
    Bitboard_t bUncontested = allBlackAttacks & ~contestedSquares;

    SerializeBySquare(wUncontested, bUncontested, wBucket, bBucket, score, boardControlBonus);

    ControlValue_t controlSums[NUM_SQUARES] = {0};

    SerializeAttacks(wPawnAttacksEast, bPawnAttacksEast, pawn_control, controlSums);
    SerializeAttacks(wPawnAttacksWest, bPawnAttacksWest, pawn_control, controlSums);

    

}