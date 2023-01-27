#include "legals.h"
#include "lookup.h"
#include "stdbool.h"
#include "pieces.h"
#include "game_state.h"

typedef Bitboard_t (*GetAttacksCallback_t)(Square_t square, Bitboard_t empty);

static Bitboard_t kscVulnerableSquares[2] = {w_ksc_vulnerable_squares, b_ksc_vulnerable_squares};
static Bitboard_t qscVulnerableSquares[2] = {w_qsc_vulnerable_squares, b_qsc_vulnerable_squares};

static Bitboard_t kscBlockableSquares[2] = {w_ksc_blockable_squares, b_ksc_blockable_squares};
static Bitboard_t qscBlockableSquares[2] = {w_qsc_blockable_squares, b_qsc_blockable_squares};

#define GetSlidingCheckers(boardInfoPtr, kingSquare, empty, enemyColor) \
    RookCaptureTargets(kingSquare, empty, (boardInfoPtr->rooks[enemyColor] | boardInfoPtr->queens[enemyColor])) | \
    BishopCaptureTargets(kingSquare, empty, (boardInfoPtr->bishops[enemyColor] | boardInfoPtr->queens[enemyColor]))

static bool KingsideCastlingIsSafe(Color_t color, Bitboard_t unsafeSquares, Bitboard_t empty) {
    return 
        !((kscVulnerableSquares[color] & unsafeSquares) ||
        (kscBlockableSquares[color] & ~empty));
}

static bool QueensideCastlingIsSafe(Color_t color, Bitboard_t unsafeSquares, Bitboard_t empty) {
    return
        !((qscVulnerableSquares[color] & unsafeSquares) ||
        (qscBlockableSquares[color] & ~empty));
}

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
    Bitboard_t result = empty_set;
    while(pieceLocations) {
        SetBits(result, GetAttacksCallback(LSB(pieceLocations), empty));
        ResetLSB(pieceLocations);
    }

    return result;
}

static Bitboard_t NonPawnUnsafeSquares(BoardInfo_t* boardInfo, Color_t enemyColor) {
    Bitboard_t empty = boardInfo->empty | (boardInfo->kings[!enemyColor]); // king does not count as blocker!

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
    Bitboard_t castlingMoves = empty_set;
    Bitboard_t kingsideSquare = ReadCastleSquares(color) & (boardInfo->kings[color] << 2);
    Bitboard_t queensideSquare = ReadCastleSquares(color) & (boardInfo->kings[color] >> 2);

    SetBits(castlingMoves, KingsideCastlingIsSafe(color, unsafeSquares, boardInfo->empty) * kingsideSquare);
    SetBits(castlingMoves, QueensideCastlingIsSafe(color, unsafeSquares, boardInfo->empty) * queensideSquare);
    return castlingMoves;
}

static Bitboard_t UpdateCheckmaskWithSliders(
    BoardInfo_t* boardInfo,
    Bitboard_t empty,
    Bitboard_t kingBitboard,
    Bitboard_t kingSquare,
    Color_t color
) 
{
    Bitboard_t checkingSliders = GetSlidingCheckers(boardInfo, kingSquare, empty, !color);

    Bitboard_t checkmask = empty_set;
    while(checkingSliders) {
        SetBits(checkmask, GetSlidingCheckmask(kingSquare, LSB(checkingSliders)));

        ResetLSB(checkingSliders);
    }

    return checkmask;
}

Bitboard_t DefineCheckmask(BoardInfo_t* boardInfo, Color_t color) {
    // assumes you are in check
    Bitboard_t kingBitboard = boardInfo->kings[color];
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);

    Bitboard_t checkmask = UpdateCheckmaskWithSliders(
        boardInfo,
        boardInfo->empty,
        kingBitboard,
        kingSquare,
        color
    );

    Bitboard_t pawnsCheckingKing = GetPawnCheckmask(kingSquare, color) & boardInfo->pawns[!color];
    SetBits(checkmask, pawnsCheckingKing);

    Bitboard_t knighsCheckingKing = GetKnightAttacks(kingSquare) & boardInfo->knights[!color];
    SetBits(checkmask, knighsCheckingKing);

    return checkmask;
}

bool IsDoubleCheck(BoardInfo_t* boardInfo, Bitboard_t checkmask, Color_t color) {
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t mask = GetKingAttacks(kingSquare) | GetKnightAttacks(kingSquare);

    return PopulationCount(mask & checkmask) > 1;
}

void DefinePinmasks(BoardInfo_t* boardInfo, Color_t color, Bitboard_t pinmaskList[NUM_DIRECTIONS]) {
    Bitboard_t kingBitboard = boardInfo->kings[color];
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);

    Bitboard_t emptyWithoutWhitePieces = (boardInfo->empty | boardInfo->allPieces[color]) & ~kingBitboard;

    Bitboard_t potentialPinmask = UpdateCheckmaskWithSliders(
        boardInfo,
        emptyWithoutWhitePieces,
        kingBitboard,
        kingSquare,
        color
    );

    for(Direction_t direction = 0; direction < NUM_DIRECTIONS; direction++) {
        Bitboard_t directionalRay = GetDirectionalRay(kingSquare, direction);
        Bitboard_t friendlyPiecesOnRay = boardInfo->allPieces[color] & directionalRay;

        bool isValidPin = PopulationCount(friendlyPiecesOnRay) == 1;
        pinmaskList[direction] = (directionalRay & potentialPinmask) * isValidPin;
    }
}