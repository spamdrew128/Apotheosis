#include <stdint.h>

#include "legals.h"
#include "lookup.h"
#include "stdbool.h"
#include "pieces.h"

typedef Bitboard_t (*GetAttacksCallback_t)(Square_t square, Bitboard_t empty);

static Bitboard_t kscVulnerableSquares[2] = {w_ksc_vulnerable_squares, b_ksc_vulnerable_squares};
static Bitboard_t qscVulnerableSquares[2] = {w_qsc_vulnerable_squares, b_qsc_vulnerable_squares};

static Bitboard_t kscBlockableSquares[2] = {w_ksc_blockable_squares, b_ksc_blockable_squares};
static Bitboard_t qscBlockableSquares[2] = {w_qsc_blockable_squares, b_qsc_blockable_squares};

typedef uint8_t PinmaskType_t;
enum pinmaskTypes{
    hv_pinmask,
    d12_pinmask
};

Bitboard_t GetSlidingCheckers(
    BoardInfo_t* boardInfo,
    Square_t kingSquare,
    Bitboard_t empty,
    Color_t enemyColor
) 
{
    return
        RookMoveTargets(kingSquare, empty, (boardInfo->rooks[enemyColor] | boardInfo->queens[enemyColor])) |
        BishopMoveTargets(kingSquare, empty, (boardInfo->bishops[enemyColor] | boardInfo->queens[enemyColor]));
}

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
    return GetKnightAttackSet(square);
}

static Bitboard_t QueenAttacks(Square_t square, Bitboard_t empty) {
    return GetRookAttackSet(square, empty) | GetBishopAttackSet(square, empty);
}

static Bitboard_t GetAllAttacks(Bitboard_t pieceLocations, Bitboard_t empty, GetAttacksCallback_t GetAttacksCallback) {
    Bitboard_t result = empty_set;
    while(pieceLocations) {
        SetBits(&result, GetAttacksCallback(LSB(pieceLocations), empty));
        ResetLSB(&pieceLocations);
    }

    return result;
}

static Bitboard_t NonPawnUnsafeSquares(BoardInfo_t* boardInfo, Color_t enemyColor) {
    Bitboard_t empty = boardInfo->empty | (boardInfo->kings[!enemyColor]); // king does not count as blocker!

    return (
        GetKingAttackSet(LSB(boardInfo->kings[enemyColor])) |
        GetAllAttacks(boardInfo->knights[enemyColor], empty, KnightAttacks) |
        GetAllAttacks(boardInfo->rooks[enemyColor], empty, GetRookAttackSet) |
        GetAllAttacks(boardInfo->bishops[enemyColor], empty, GetBishopAttackSet) |
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

bool CanCastleQueenside(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, CastleRights_t castleRights, Color_t color) {
    return QueensideCastlingIsSafe(color, unsafeSquares, boardInfo->empty) && ReadQueensideCastleRights(castleRights, color);
}

bool CanCastleKingside(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, CastleRights_t castleRights, Color_t color) {
    return KingsideCastlingIsSafe(color, unsafeSquares, boardInfo->empty) && ReadKingsideCastleRights(castleRights, color);
}

static Bitboard_t CalculateSliderCheckmask(
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
        SetBits(&checkmask, GetSlidingCheckmask(kingSquare, LSB(checkingSliders)));

        ResetLSB(&checkingSliders);
    }

    return checkmask;
}

Bitboard_t DefineCheckmask(BoardInfo_t* boardInfo, Color_t color) {
    // assumes you are in check
    Bitboard_t kingBitboard = boardInfo->kings[color];
    Bitboard_t kingSquare = KingSquare(boardInfo, color);

    Bitboard_t checkmask = CalculateSliderCheckmask(
        boardInfo,
        boardInfo->empty,
        kingBitboard,
        kingSquare,
        color
    );

    Bitboard_t pawnsCheckingKing = GetPawnCheckmask(kingSquare, color) & boardInfo->pawns[!color];
    SetBits(&checkmask, pawnsCheckingKing);

    Bitboard_t knighsCheckingKing = GetKnightAttackSet(kingSquare) & boardInfo->knights[!color];
    SetBits(&checkmask, knighsCheckingKing);

    return checkmask;
}

bool InCheck(Bitboard_t kingBitboard, Bitboard_t unsafeSquares) {
    return unsafeSquares & kingBitboard;
}

bool IsDoubleCheck(BoardInfo_t* boardInfo, Bitboard_t checkmask, Color_t color) {
    Bitboard_t kingSquare = KingSquare(boardInfo, color);
    Bitboard_t mask = GetKingAttackSet(kingSquare) | GetKnightAttackSet(kingSquare);

    return PopulationCount(mask & checkmask) > 1;
}

static Bitboard_t CalculateDirectionalPinmask(
    Bitboard_t potentialPinmaskSquares,
    Bitboard_t allPieces,
    Square_t kingSquare,
    PinmaskType_t pinmaskType  
) 
{
    Bitboard_t pinmask = empty_set;

    for(Direction_t direction = pinmaskType; direction < NUM_DIRECTIONS; direction += 2) {
        Bitboard_t directionalPinmask = GetDirectionalRay(kingSquare, direction) & potentialPinmaskSquares;
        Bitboard_t affectedPieces = 
            directionalPinmask & allPieces;

        bool isValidPin = PopulationCount(affectedPieces) == 1;
        pinmask |= directionalPinmask * isValidPin;
    }

    return pinmask;
}

PinmaskContainer_t DefinePinmasks(BoardInfo_t* boardInfo, Color_t color) {
    Bitboard_t kingBitboard = boardInfo->kings[color];
    Bitboard_t kingSquare = KingSquare(boardInfo, color);

    Bitboard_t emptyIfNoFriendlyPiecesOtherThanKing = 
        (boardInfo->empty | boardInfo->allPieces[color]) & ~kingBitboard;

    Bitboard_t potentialPinmaskSquares = CalculateSliderCheckmask(
        boardInfo,
        emptyIfNoFriendlyPiecesOtherThanKing,
        kingBitboard,
        kingSquare,
        color
    );

    PinmaskContainer_t pinmasks;
    pinmasks.hv = CalculateDirectionalPinmask(
        potentialPinmaskSquares,
        boardInfo->allPieces[color],
        kingSquare,
        hv_pinmask
    );

    pinmasks.d12 = CalculateDirectionalPinmask(
        potentialPinmaskSquares,
        boardInfo->allPieces[color],
        kingSquare,
        d12_pinmask
    );

    pinmasks.all = pinmasks.hv | pinmasks.d12;

    return pinmasks;
}

bool EastEnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t friendlyPawnLocation, Color_t color) {
    Bitboard_t enemyPawnLocation = EastOne(friendlyPawnLocation);
    Square_t kingSquare = KingSquare(boardInfo, color);

    Bitboard_t enemyHvSliders = boardInfo->queens[!color] | boardInfo->rooks[!color];

    return !RookMoveTargets(
        kingSquare,
        boardInfo->empty | enemyPawnLocation | friendlyPawnLocation,
        enemyHvSliders
    );
}

bool WestEnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t friendlyPawnLocation, Color_t color) {
    Bitboard_t enemyPawnLocation = WestOne(friendlyPawnLocation);
    Square_t kingSquare = KingSquare(boardInfo, color);

    Bitboard_t enemyHvSliders = boardInfo->queens[!color] | boardInfo->rooks[!color];

    return !RookMoveTargets(
        kingSquare,
        boardInfo->empty | enemyPawnLocation | friendlyPawnLocation,
        enemyHvSliders
    );
}