#include "legals.h"
#include "movegen.h"
#include "pieces.h"

typedef Bitboard_t (*UnsafeSquaresCallback_t)(BoardInfo_t* boardInfo);
static UnsafeSquaresCallback_t UnsafeSquaresCallbacks[2] = { WhiteUnsafeSquares, BlackUnsafeSquares };

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

static void AddKingMoves(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingMoves = KingMoveTargets(LSB(boardInfo->kings[color]), boardInfo->empty);
    Bitboard_t kingLegalMoves = KingLegalMoves(kingMoves, unsafeSquares);
    SerializeMovesIntoMovelist(moveList, kingLegalMoves, boardInfo->kings[color], king);
}

static void AddKingCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingCaptures = KingCaptureTargets(LSB(boardInfo->kings[color]), boardInfo->allPieces[!color]);
    Bitboard_t kingLegalCaptures = KingLegalMoves(kingCaptures, unsafeSquares);
    SerializeMovesIntoMovelist(moveList, kingLegalCaptures, boardInfo->kings[color], king);
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
    Bitboard_t horizontalPins = pinmaskList[E] | pinmaskList[W];
    Bitboard_t verticalPins = pinmaskList[N] | pinmaskList[S];
    Bitboard_t d1Pins = pinmaskList[NE] | pinmaskList[SW];
    Bitboard_t d2Pins = pinmaskList[SW] | pinmaskList[SE];
    Bitboard_t allPins = horizontalPins | verticalPins | d1Pins | d2Pins;
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