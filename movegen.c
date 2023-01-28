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

#define SerializePositionsIntoMoves(_positions, ...) \
    do { \
        while(_positions) { \
            __VA_ARGS__ \
            ResetLSB(_positions); \
        } \
    } while(0) 

#define SerializeMovesIntoMoveList(_moveListPtr, _moves, ...) \
    do { \
        while(_moves) { \
            AddNewMove(_moveListPtr); \
            __VA_ARGS__ \
            ResetLSB(_moves); \
        } \
    } while(0)

// static void SerializeMovesIntoMovelist(MoveList_t* moveList, Bitboard_t newMoves, Square_t fromSquare, Piece_t piece) {
//     while(newMoves) {
//         AddNewMove(moveList);

//         WriteToSquare(CurrentMove(moveList), LSB(newMoves));
//         WriteFromSquare(CurrentMove(moveList), fromSquare);
//         WriteSpecialFlag(CurrentMove(moveList), no_flag);

//         ResetLSB(newMoves);
//     }
// }

// static void SerializeSliderPostionsIntoMoves(
//     MoveList_t* moveList,
//     Bitboard_t sliders,
//     Bitboard_t restriction,
//     Bitboard_t empty,
//     Bitboard_t enemyPieces,
//     Piece_t pieceType,
//     SliderCaptureTargetsCallback_t SliderCaptureTargetsCallback
// ) 
// {
//     while(sliders) {
//         Square_t sliderSquare = LSB(sliders);
//         Bitboard_t sliderCaptures = 
//             SliderCaptureTargetsCallback(sliderSquare, empty, enemyPieces) & restriction;

//         SerializeMovesIntoMovelist(moveList, sliderCaptures, sliderSquare, pieceType);

//         ResetLSB(sliders);
//     }
// }

static void AddKingMoves(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t kingMoves = KingMoveTargets(kingSquare, boardInfo->empty);
    Bitboard_t kingLegalMoves = KingLegalMoves(kingMoves, unsafeSquares);
    
    SerializeMovesIntoMoveList(moveList, kingLegalMoves, {
        WriteToSquare(CurrentMove(moveList), LSB(kingLegalMoves));
        WriteFromSquare(CurrentMove(moveList), kingSquare); 
    });
}

static void AddKingCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, Bitboard_t unsafeSquares, Color_t color) {
    Bitboard_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t kingCaptures = KingCaptureTargets(kingSquare, boardInfo->allPieces[!color]);
    Bitboard_t kingLegalCaptures = KingLegalMoves(kingCaptures, unsafeSquares);

    SerializeMovesIntoMoveList(moveList, kingLegalCaptures, {
        WriteToSquare(CurrentMove(moveList), LSB(kingLegalCaptures));
        WriteFromSquare(CurrentMove(moveList), kingSquare); 
    });
}

static void AddKnightCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, PinmaskContainer_t pinmasks, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeKnights = boardInfo->knights[color] & ~pinmasks.allPinmask;
    Bitboard_t enemyPieces = boardInfo->allPieces[!color];

    while(freeKnights) {
        Square_t knightSquare = LSB(freeKnights);
        Bitboard_t knightCaptures = KnightCaptureTargets(knightSquare, boardInfo->allPieces[!color]) & checkmask;

        SerializeMovesIntoMovelist(moveList, knightCaptures, knightSquare, knight);

        ResetLSB(freeKnights);
    }

    SerializePositionsIntoMoves(freeKnights, {\
        Bitboard_t knightSquare = LSB(freeKnights);
        Bitboard_t moves = KnightCaptureTargets(LSB(freeKnights), enemyPieces) & checkmask;
        SerializeMovesIntoMoveList(moveList, moves, {
            WriteToSquare(CurrentMove(moveList), LSB(moves));
            WriteFromSquare(CurrentMove(moveList),knightSquare);
        });
    });
}

static void AddBishopCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, PinmaskContainer_t pinmasks, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeBishops = boardInfo->bishops[color] & ~pinmasks.allPinmask;
    Bitboard_t pinnedBishops = boardInfo->bishops[color] & pinmasks.d12Pinmask;
    
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
        checkmask & pinmasks.d12Pinmask,
        boardInfo->empty,
        enemy,
        bishop,
        BishopCaptureTargets
    );
}

static void AddRookCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, PinmaskContainer_t pinmasks, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeRooks = boardInfo->rooks[color] & ~pinmasks.allPinmask;
    Bitboard_t pinnedRooks = boardInfo->rooks[color] & pinmasks.hvPinmask;
    
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
        checkmask & pinmasks.hvPinmask,
        boardInfo->empty,
        enemy,
        rook,
        RookCaptureTargets
    );
}

static void AddQueenCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, PinmaskContainer_t pinmasks, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeQueens = boardInfo->queens[color] & ~pinmasks.allPinmask;
    Bitboard_t hvPinnedQueens = boardInfo->queens[color] & pinmasks.hvPinmask;
    Bitboard_t d12PinnedQueens = boardInfo->queens[color] & pinmasks.d12Pinmask;
    
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
        checkmask & pinmasks.hvPinmask,
        boardInfo->empty,
        enemy,
        queen,
        RookCaptureTargets
    );

    SerializeSliderPostionsIntoMoves(
        moveList,
        d12PinnedQueens,
        checkmask & pinmasks.hvPinmask,
        boardInfo->empty,
        enemy,
        queen,
        BishopCaptureTargets
    );
}

static void SerializePawnMoves(
    MoveList_t* moveList,
    Bitboard_t sliders,
    Bitboard_t restriction,
    Bitboard_t empty,
    Bitboard_t enemyPieces,
    Piece_t pieceType,
    SliderCaptureTargetsCallback_t SliderCaptureTargetsCallback
) 
{
    
}

static void AddPawnCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, PinmaskContainer_t pinmasks, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freePawns = boardInfo->pawns[color] & ~pinmasks.allPinmask;
    Bitboard_t pinnedPawns = boardInfo->pawns[color] & ~pinmasks.d12Pinmask;

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

    PinmaskContainer_t pinmasks = DefinePinmasks(boardInfo, color);

    AddKnightCaptures(moveList, boardInfo, pinmasks, checkmask, color);
    AddBishopCaptures(moveList, boardInfo, pinmasks, checkmask, color);
    AddRookCaptures(moveList, boardInfo, pinmasks, checkmask, color);
    AddQueenCaptures(moveList, boardInfo, pinmasks, checkmask, color);
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