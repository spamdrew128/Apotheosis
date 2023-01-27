#include "legals.h"
#include "movegen.h"
#include "pieces.h"

typedef Bitboard_t (*UnsafeSquaresCallback_t)(BoardInfo_t* boardInfo);
static UnsafeSquaresCallback_t UnsafeSquaresCallbacks[2] = { WhiteUnsafeSquares, BlackUnsafeSquares };

typedef Bitboard_t (*SliderCaptureTargetsCallback_t)(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces);

#define CurrentMove(moveListPtr) \
    moveListPtr->moves[moveListPtr->maxIndex]

#define AddNewMove(moveListPtr) \
    (moveListPtr->maxIndex)++; InitalizeMove(CurrentMove(moveListPtr))

static void SerializeMovesIntoMovelist(MoveList_t* moveList, Bitboard_t newMoves, Square_t fromSquare, Piece_t piece) {
    while(newMoves) {
        AddNewMove(moveList);

        WriteToSquare(CurrentMove(moveList), LSB(newMoves));
        WriteFromSquare(CurrentMove(moveList), fromSquare);
        WriteSpecialFlag(CurrentMove(moveList), no_flag);

        ResetLSB(newMoves);
    }
}

static void SerializeSliderPostionsIntoMoves(
    MoveList_t* moveList,
    Bitboard_t sliders,
    Bitboard_t restriction,
    Bitboard_t empty,
    Bitboard_t enemyPieces,
    Piece_t pieceType,
    SliderCaptureTargetsCallback_t SliderCaptureTargetsCallback
) 
{
    while(sliders) {
        Square_t sliderSquare = LSB(sliders);
        Bitboard_t sliderCaptures = 
            SliderCaptureTargetsCallback(sliderSquare, empty, enemyPieces) & restriction;

        SerializeMovesIntoMovelist(moveList, sliderCaptures, sliderSquare, pieceType);

        ResetLSB(sliders);
    }
}

static void AddKingMoves(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t kingMoves = KingMoveTargets(kingSquare, boardInfo->empty);
    Bitboard_t kingLegalMoves = KingLegalMoves(kingMoves, unsafeSquares);
    SerializeMovesIntoMovelist(moveList, kingLegalMoves, kingSquare, king);
}

static void AddKingCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t kingCaptures = KingCaptureTargets(kingSquare, boardInfo->allPieces[!color]);
    Bitboard_t kingLegalCaptures = KingLegalMoves(kingCaptures, unsafeSquares);
    SerializeMovesIntoMovelist(moveList, kingLegalCaptures, kingSquare, king);
}

static void AddKnightCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t allPins, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeKnights = boardInfo->knights[color] & ~allPins;

    while(freeKnights) {
        Square_t knightSquare = LSB(freeKnights);
        Bitboard_t knightCaptures = KnightCaptureTargets(knightSquare, boardInfo->allPieces[!color]) & checkmask;

        SerializeMovesIntoMovelist(moveList, knightCaptures, knightSquare, knight);

        ResetLSB(freeKnights);
    }
}

static void AddBishopCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t d12Pins, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeBishops = boardInfo->bishops[color] & ~d12Pins;
    Bitboard_t pinnedBishops = boardInfo->bishops[color] & d12Pins;
    
    Bitboard_t enemy = boardInfo->allPieces[!color];

    SerializeSliderPostionsIntoMoves(
        moveList,
        freeBishops,
        checkmask,
        boardInfo->empty,
        enemy,
        bishop,
        BishopCaptureTargets
    );

    SerializeSliderPostionsIntoMoves(
        moveList,
        pinnedBishops,
        checkmask & d12Pins,
        boardInfo->empty,
        enemy,
        bishop,
        BishopCaptureTargets
    );
}

static void AddRookCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t hvPins, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeRooks = boardInfo->rooks[color] & ~hvPins;
    Bitboard_t pinnedRooks = boardInfo->rooks[color] & hvPins;
    
    Bitboard_t enemy = boardInfo->allPieces[!color];

    SerializeSliderPostionsIntoMoves(
        moveList,
        freeRooks,
        checkmask,
        boardInfo->empty,
        enemy,
        rook,
        RookCaptureTargets
    );

    SerializeSliderPostionsIntoMoves(
        moveList,
        pinnedRooks,
        checkmask & hvPins,
        boardInfo->empty,
        enemy,
        rook,
        RookCaptureTargets
    );
}

static void AddQueenCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t hvPins, Bitboard_t d12Pins, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeQueens = boardInfo->queens[color] & ~(hvPins | d12Pins);
    Bitboard_t hvPinnedQueens = boardInfo->queens[color] & hvPins;
    Bitboard_t d12PinnedQueens = boardInfo->queens[color] & d12Pins;
    
    Bitboard_t enemy = boardInfo->allPieces[!color];

    SerializeSliderPostionsIntoMoves(
        moveList,
        freeQueens,
        checkmask,
        boardInfo->empty,
        enemy,
        queen,
        QueenCaptureTargets
    );

    SerializeSliderPostionsIntoMoves(
        moveList,
        hvPinnedQueens,
        checkmask & hvPins,
        boardInfo->empty,
        enemy,
        queen,
        RookCaptureTargets
    );

    SerializeSliderPostionsIntoMoves(
        moveList,
        d12PinnedQueens,
        checkmask & hvPins,
        boardInfo->empty,
        enemy,
        queen,
        BishopCaptureTargets
    );
}

static void AddPawnCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t d12Pins, Bitboard_t allPins, Bitboard_t checkmask, Color_t color) {
    // Bitboard_t pawnCaptures = PawnCa
}

void CapturesMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color) {
    moveList->maxIndex = 0;

    UnsafeSquaresCallback_t Callback = UnsafeSquaresCallbacks[color];
    Bitboard_t unsafeSquares = Callback(boardInfo);

    AddKingCaptures(moveList, boardInfo, unsafeSquares, color); 

    Bitboard_t checkmask = full_set;
    if(InCheck(boardInfo, unsafeSquares, color)) {
        checkmask = DefineCheckmask(boardInfo, color);
        if(IsDoubleCheck(boardInfo, checkmask, color)) {
            return;
        }
    } 
    
    Bitboard_t pinmaskList[NUM_DIRECTIONS];
    DefinePinmasks(boardInfo, color, pinmaskList);
    Bitboard_t hvPins = pinmaskList[N] | pinmaskList[E] | pinmaskList[S] | pinmaskList[W];
    Bitboard_t d12Pins = pinmaskList[NE] | pinmaskList[SE] | pinmaskList[NW] | pinmaskList[SW];
    Bitboard_t allPins = hvPins | d12Pins;
    
    AddKnightCaptures(moveList, boardInfo, allPins, checkmask, color);
    AddBishopCaptures(moveList, boardInfo, d12Pins, checkmask, color);
    AddRookCaptures(moveList, boardInfo, hvPins, checkmask, color);
    AddQueenCaptures(moveList, boardInfo, hvPins, d12Pins, checkmask, color);
}

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color) {
    moveList->maxIndex = 0;

    UnsafeSquaresCallback_t Callback = UnsafeSquaresCallbacks[color];
    Bitboard_t unsafeSquares = Callback(boardInfo);

    AddKingCaptures(moveList, boardInfo, unsafeSquares, color);
    AddKingMoves(moveList, boardInfo, unsafeSquares, color);

    Bitboard_t checkmask = full_set;
    if(InCheck(boardInfo, unsafeSquares, color)) {
        checkmask = DefineCheckmask(boardInfo, color);
        if(IsDoubleCheck(boardInfo, checkmask, color)) {
            return;
        }
    }
}