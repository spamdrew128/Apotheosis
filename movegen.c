#include "legals.h"
#include "movegen.h"
#include "pieces.h"

typedef Bitboard_t (*UnsafeSquaresCallback_t)(BoardInfo_t* boardInfo);
static UnsafeSquaresCallback_t UnsafeSquaresCallbacks[2] = { WhiteUnsafeSquares, BlackUnsafeSquares };

static void SerializeMovesIntoMovelist(MoveList_t* moveList, Bitboard_t newMoves, Bitboard_t fromSquare, Piece_t piece) {
    while(newMoves) {
        (moveList->numMoves)++;
        CurrentMove(moveList).piece = piece;
        CurrentMove(moveList).captureTargetType = none;
        CurrentMove(moveList).fromSquare = fromSquare;
        CurrentMove(moveList).toSquare = IsolateLSB(newMoves);
        CurrentMove(moveList).promotionType = none;

        ResetLSB(newMoves);
    }
}

static void _SerializeCapturesHelper(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t newCaptures,
    Bitboard_t fromSquare,
    Piece_t piece,
    Piece_t targetType
) 
{
    while(newCaptures) {
        (moveList->numMoves)++;
        CurrentMove(moveList).piece = piece;
        CurrentMove(moveList).captureTargetType = targetType;
        CurrentMove(moveList).fromSquare = fromSquare;
        CurrentMove(moveList).toSquare = IsolateLSB(newCaptures);
        CurrentMove(moveList).promotionType = none;

        ResetLSB(newCaptures);
    }
}

static void SerializeCapturesIntoMovelist(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t newCaptures,
    Bitboard_t fromSquare,
    Piece_t piece,
    Color_t color
) 
{
    Color_t enemyColor = !color;
    _SerializeCapturesHelper(
        moveList,
        boardInfo,
        boardInfo->pawns[enemyColor] | newCaptures,
        fromSquare,
        piece,
        pawn
    );
    _SerializeCapturesHelper(
        moveList,
        boardInfo,
        boardInfo->knights[enemyColor] | newCaptures,
        fromSquare,
        piece,
        knight
    );
    _SerializeCapturesHelper(
        moveList,
        boardInfo,
        boardInfo->bishops[enemyColor] | newCaptures,
        fromSquare,
        piece,
        bishop
    );
    _SerializeCapturesHelper(
        moveList,
        boardInfo,
        boardInfo->rooks[enemyColor] | newCaptures,
        fromSquare,
        piece,
        rook
    );
    _SerializeCapturesHelper(
        moveList,
        boardInfo,
        boardInfo->rooks[enemyColor] | newCaptures,
        fromSquare,
        piece,
        queen
    );
}

static void AddKingMoves(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingMoves = KingMoveTargets(LSB(boardInfo->kings[color]), boardInfo->empty);
    Bitboard_t kingLegalMoves = KingLegalMoves(kingMoves, unsafeSquares);
    SerializeMovesIntoMovelist(moveList, kingLegalMoves, boardInfo->kings[color], king);
}

static void AddKingCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingCaptures = KingCaptureTargets(LSB(boardInfo->kings[color]), boardInfo->allPieces[!color]);
    Bitboard_t kingLegalCaptures = KingLegalMoves(kingCaptures, unsafeSquares);
    SerializeCapturesIntoMovelist(
        moveList,
        boardInfo,
        kingLegalCaptures,
        boardInfo->kings[color],
        king,
        color
    );
}

void CapturesMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color) {
    moveList->numMoves = 0;

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
}

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color) {
    moveList->numMoves = 0;
}