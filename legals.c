#include "legals.h"
#include "lookup.h"

typedef Bitboard_t (*GetAttacksCallback_t)(Square_t, Bitboard_t);

static Bitboard_t kingsideCastleMasks[2] = {white_kingside_castle_mask, black_kingside_castle_mask};
static Bitboard_t queensideCastleMasks[2] = {white_queenside_castle_mask, black_queenside_castle_mask};

#define KingsideCastlingIsSafe(color, unsafeSquares) !(kingsideCastleMasks[color] & unsafeSquares)

#define QueensideCastlingIsSafe(color, unsafeSquares) !(queensideCastleMasks[color] & unsafeSquares)

static Bitboard_t KnightAttacks(Square_t square, Bitboard_t empty) {
    (void)empty;
    return GetKnightAttacks(square);
}

static Bitboard_t RookAttacks(Square_t square, Bitboard_t empty) {
    MagicEntry_t magicEntry = GetRookMagicEntry(square);
    Bitboard_t blockers = magicEntry.mask & ~empty;
    return GetSlidingAttackSet(magicEntry, blockers);
}

static Bitboard_t BishopAttacks(Square_t square, Bitboard_t empty) {
    MagicEntry_t magicEntry = GetBishopMagicEntry(square);
    Bitboard_t blockers = magicEntry.mask & ~empty;
    return GetSlidingAttackSet(magicEntry, blockers);
}

static Bitboard_t QueenAttacks(Square_t square, Bitboard_t empty) {
    return RookAttacks(square, empty) | BishopAttacks(square, empty);
}

static Bitboard_t GetAllAttacks(Bitboard_t pieceLocations, Bitboard_t empty, GetAttacksCallback_t GetAttacksCallback) {
    Bitboard_t result = 0;
    while(pieceLocations) {
        result |= GetAttacksCallback(LSB(pieceLocations), empty);
        pieceLocations &= pieceLocations - 1;
    }

    return result;
}

static Bitboard_t NonPawnUnsafeSquares(BoardInfo_t* boardInfo, Color_t enemyColor) {
    Bitboard_t empty = boardInfo->empty & ~(boardInfo->kings[!enemyColor]);

    return (
        GetKingAttacks(LSB(boardInfo->kings[enemyColor])) |
        GetAllAttacks(boardInfo->knights[enemyColor], empty, KnightAttacks) |
        GetAllAttacks(boardInfo->rooks[enemyColor], empty, RookAttacks) |
        GetAllAttacks(boardInfo->bishops[enemyColor], empty, BishopAttacks) |
        GetAllAttacks(boardInfo->queens[enemyColor], empty, QueenAttacks)
    );
}

Bitboard_t WhiteUnsafeSquares(BoardInfo_t* boardInfo) {
    return (
        SoEaOne(boardInfo->pawns[black]) |
        SoWeOne(boardInfo->pawns[black]) |
        NonPawnUnsafeSquares(boardInfo, black)
    );
}

Bitboard_t BlackUnsafeSquares(BoardInfo_t* boardInfo) {
    return (
        NoEaOne(boardInfo->pawns[white]) |
        NoWeOne(boardInfo->pawns[white]) |
        NonPawnUnsafeSquares(boardInfo, white)
    );
}

Bitboard_t KingLegalMoves(Bitboard_t kingMoves, Bitboard_t unsafeSquares) {
    return kingMoves & ~unsafeSquares;
}

Bitboard_t CastlingMoves(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t castlingMoves = C64(0);
    Bitboard_t kingsideSquare = boardInfo->castleSquares[color] & (boardInfo->kings[color] << 2);
    Bitboard_t queensideSquare = boardInfo->castleSquares[color] & (boardInfo->kings[color] >> 2);

    castlingMoves |= KingsideCastlingIsSafe(color, unsafeSquares) * kingsideSquare;
    castlingMoves |= QueensideCastlingIsSafe(color, unsafeSquares) * queensideSquare;
    return castlingMoves;
}