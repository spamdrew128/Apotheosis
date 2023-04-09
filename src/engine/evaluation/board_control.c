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
    Bitboard_t wPieces,
    Bitboard_t bPieces,
    Bitboard_t wEmpty,
    Bitboard_t bEmpty,
    const Bitboard_t contestedSquares,
    ControlValue_t controlValue,
    ControlValue_t controlSums[NUM_SQUARES]
)
{
    while(wPieces) {
        Square_t sq = LSB(wPieces);
        Bitboard_t attacks = GetRookAttackSet(sq, wEmpty) & contestedSquares;
        SerializeAttacks(attacks, controlValue, controlSums);
        ResetLSB(&wPieces);
    }
    while(bPieces) {
        Square_t sq = LSB(bPieces);
        Bitboard_t attacks = GetRookAttackSet(sq, bEmpty) & contestedSquares;
        SerializeAttacks(attacks, -controlValue, controlSums);
        ResetLSB(&bPieces);
    }
}

static void D12SliderControl(
    Bitboard_t wPieces,
    Bitboard_t bPieces,
    Bitboard_t wEmpty,
    Bitboard_t bEmpty,
    const Bitboard_t contestedSquares,
    ControlValue_t controlValue,
    ControlValue_t controlSums[NUM_SQUARES]
)
{
    while(wPieces) {
        Square_t sq = LSB(wPieces);
        Bitboard_t attacks = GetBishopAttackSet(sq, wEmpty) & contestedSquares;
        SerializeAttacks(attacks, controlValue, controlSums);
        ResetLSB(&wPieces);
    }
    while(bPieces) {
        Square_t sq = LSB(bPieces);
        Bitboard_t attacks = GetBishopAttackSet(sq, bEmpty) & contestedSquares;
        SerializeAttacks(attacks, -controlValue, controlSums);
        ResetLSB(&bPieces);
    }
}

static void KnightControl(
    Bitboard_t wPieces,
    Bitboard_t bPieces,
    const Bitboard_t contestedSquares,
    ControlValue_t controlValue,
    ControlValue_t controlSums[NUM_SQUARES]
)
{
    while(wPieces) {
        Square_t sq = LSB(wPieces);
        Bitboard_t attacks = GetKnightAttackSet(sq) & contestedSquares;
        SerializeAttacks(attacks, controlValue, controlSums);
        ResetLSB(&wPieces);
    }
    while(bPieces) {
        Square_t sq = LSB(bPieces);
        Bitboard_t attacks = GetKnightAttackSet(sq) & contestedSquares;
        SerializeAttacks(attacks, -controlValue, controlSums);
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

    SerializeAttacks(wPawnAttacksEast & contestedSquares, pawn_control, controlSums);
    SerializeAttacks(bPawnAttacksWest & contestedSquares, -pawn_control, controlSums);
    SerializeAttacks(wPawnAttacksEast & contestedSquares, pawn_control, controlSums);
    SerializeAttacks(bPawnAttacksWest & contestedSquares, -pawn_control, controlSums);

    Bitboard_t wHvEmpty = boardInfo->empty | boardInfo->rooks[white] | boardInfo->queens[white];
    Bitboard_t bHvEmpty = boardInfo->empty | boardInfo->rooks[black] | boardInfo->queens[black];

    HvSliderControl(boardInfo->rooks[white], boardInfo->rooks[black], wHvEmpty, bHvEmpty, contestedSquares, rook_control, controlSums);
    HvSliderControl(boardInfo->queens[white], boardInfo->queens[black], wHvEmpty, bHvEmpty, contestedSquares, queen_control, controlSums);

    Bitboard_t wD12Empty = boardInfo->empty | boardInfo->bishops[white] | boardInfo->queens[white];
    Bitboard_t bD12Empty = boardInfo->empty | boardInfo->bishops[black] | boardInfo->queens[black];

    D12SliderControl(boardInfo->bishops[white], boardInfo->bishops[black], wD12Empty, bD12Empty, contestedSquares, bishop_control, controlSums);
    D12SliderControl(boardInfo->queens[white], boardInfo->queens[black], wD12Empty, bD12Empty, contestedSquares, queen_control, controlSums);

    KnightControl(boardInfo->knights[white], boardInfo->knights[black], contestedSquares, knight_control, controlSums);

    while(contestedSquares) {
        Square_t sq = LSB(contestedSquares);
        ControlValue_t value = controlSums[sq];
        if(value > 0) {
            *score += boardControlBonus[wBucket][MIRROR(sq)];
        }
        if(value < 0) {
            *score += boardControlBonus[bBucket][sq];
        }
        ResetLSB(&contestedSquares);
    }
}