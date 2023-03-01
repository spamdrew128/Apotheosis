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

static Bitboard_t QueenAttacks(Square_t square, Bitboard_t empty) {
    return GetRookAttackSet(square, empty) | GetBishopAttackSet(square, empty);
}

static Bitboard_t AttackedSquares(BoardInfo_t* boardInfo, Bitboard_t empty, Color_t color) {
    Bitboard_t attackedSquares = GetKingAttackSet(KingSquare(boardInfo, color));

    if(color == white) {
        attackedSquares |= 
            NoEaOne(boardInfo->pawns[white]) | 
            NoWeOne(boardInfo->pawns[white]);
    } else {
        attackedSquares |= 
            SoEaOne(boardInfo->pawns[black]) |
            SoWeOne(boardInfo->pawns[black]);
    }

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
}

Bitboard_t UnsafeSquares(BoardInfo_t* boardInfo, Color_t color) {
    // king does not count as blocker in this case!
    Bitboard_t empty = boardInfo->empty | (boardInfo->kings[color]); 
    return AttackedSquares(boardInfo, empty, !color);
}

Bitboard_t KingLegalMoves(Bitboard_t kingMoves, Bitboard_t unsafeSquares) {
    return kingMoves & ~unsafeSquares;
}

bool CanCastleQueenside(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Bitboard_t castlingRights, Color_t color) {
    bool queensideSquareExists = castlingRights & GenShiftWest(boardInfo->kings[color], 2);
    return QueensideCastlingIsSafe(color, unsafeSquares, boardInfo->empty) && queensideSquareExists;
}

bool CanCastleKingside(BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Bitboard_t castlingRights, Color_t color) {
    bool kingsideSquareExists = castlingRights & GenShiftEast(boardInfo->kings[color], 2);
    return KingsideCastlingIsSafe(color, unsafeSquares, boardInfo->empty) && kingsideSquareExists;
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

    return PopCount(mask & checkmask) > 1;
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

        bool isValidPin = PopCount(affectedPieces) == 1;
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