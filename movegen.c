#include "legals.h"
#include "movegen.h"
#include "pieces.h"
#include "lookup.h"
#include "game_state.h"

typedef Bitboard_t (*UnsafeSquaresCallback_t)(BoardInfo_t* boardInfo);
static UnsafeSquaresCallback_t UnsafeSquaresCallbacks[2] = { WhiteUnsafeSquares, BlackUnsafeSquares };

typedef Bitboard_t (*SliderCaptureTargetsCallback_t)(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces);

static Move_t* CurrentMove(MoveList_t* moveList) {
    return &(moveList->moves[moveList->maxIndex]);
}

static void InitializeNewMove(MoveList_t* moveList) {
    (moveList->maxIndex)++;
    InitMove(CurrentMove(moveList));
}

static void SerializeNormalMoves(
    MoveList_t* moveList,
    Square_t pieceSquare,
    Bitboard_t moves
) 
{
    while(moves) {
        InitializeNewMove(moveList);
        Move_t* current = CurrentMove(moveList);

        WriteToSquare(current, LSB(moves));
        WriteFromSquare(current, pieceSquare);

        ResetLSB(&moves);
    }
}

#define SerializePositionsIntoMoves(_positions, ...) \
    do { \
        while(_positions) { \
            __VA_ARGS__ \
            ResetLSB(&_positions); \
        } \
    } while(0)

static void AddKingMoves(
    MoveList_t* moveList,
    Bitboard_t kingSquare,
    Bitboard_t kingPsuedolegals,
    Bitboard_t unsafeSquares,
    Bitboard_t empty
)
{
    Bitboard_t kingLegalMoves = KingLegalMoves(kingPsuedolegals, unsafeSquares);
    
    SerializeNormalMoves(moveList, kingSquare, kingLegalMoves);
}

static void _AddCastlingMovesHelper(MoveList_t* moveList, Square_t kingSquare, Square_t castleSquare) {
    InitializeNewMove(moveList);
    Move_t* current = CurrentMove(moveList);

    WriteToSquare(current, castleSquare);
    WriteFromSquare(current, kingSquare);
    WriteSpecialFlag(current, castle_flag);
}

static void AddCastlingMoves(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t unsafeSquares,
    Square_t kingSquare,
    Color_t color
)
{
    if(CanCastleKingside(boardInfo, unsafeSquares, color)) {
        _AddCastlingMovesHelper(moveList, kingSquare, GetKingsideCastleSquare(color));
    }
    
    if(CanCastleQueenside(boardInfo, unsafeSquares, color)) {
        _AddCastlingMovesHelper(moveList, kingSquare, GetQueensideCastleSquare(color));
    }
}

static void AddKnightCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo, PinmaskContainer_t pinmasks, Bitboard_t checkmask, Color_t color) {
    Bitboard_t freeKnights = boardInfo->knights[color] & ~pinmasks.all;
    Bitboard_t enemyPieces = boardInfo->allPieces[!color];

    SerializePositionsIntoMoves(freeKnights, {
        Bitboard_t knightSquare = LSB(freeKnights);
        Bitboard_t moves = KnightCaptureTargets(knightSquare, enemyPieces) & checkmask;
        SerializeNormalMoves(moveList, knightSquare, moves);
    });
}

static void AddBishopCaptures(
    MoveList_t* moveList,
    Bitboard_t freeBishops,
    Bitboard_t d12PinnedBishops,
    Bitboard_t checkmask,
    Bitboard_t empty,
    Bitboard_t enemyPieces,
    PinmaskContainer_t pinmasks
) 
{
    SerializePositionsIntoMoves(freeBishops, {
        Bitboard_t bishopSquare = LSB(freeBishops);
        Bitboard_t moves = BishopCaptureTargets(bishopSquare, empty, enemyPieces) & checkmask;
        SerializeNormalMoves(moveList, bishopSquare, moves);
    });

    SerializePositionsIntoMoves(d12PinnedBishops, {
        Bitboard_t bishopSquare = LSB(d12PinnedBishops);
        Bitboard_t moves = BishopCaptureTargets(bishopSquare, empty, enemyPieces) & checkmask & pinmasks.d12;
        SerializeNormalMoves(moveList, bishopSquare, moves);
    });
}

static void AddRookCaptures(
    MoveList_t* moveList,
    Bitboard_t freeRooks,
    Bitboard_t hvPinnedRooks,
    Bitboard_t checkmask,
    Bitboard_t empty,
    Bitboard_t enemyPieces,
    PinmaskContainer_t pinmasks
) 
{
    SerializePositionsIntoMoves(freeRooks, {
        Bitboard_t rookSquare = LSB(freeRooks);
        Bitboard_t moves = RookCaptureTargets(rookSquare, empty, enemyPieces) & checkmask;
        SerializeNormalMoves(moveList, rookSquare, moves);
    });

    SerializePositionsIntoMoves(hvPinnedRooks, {
        Bitboard_t rookSquare = LSB(hvPinnedRooks);
        Bitboard_t moves = RookCaptureTargets(rookSquare, empty, enemyPieces) & checkmask & pinmasks.hv;
        SerializeNormalMoves(moveList, rookSquare, moves);
    });
}

static void AddQueenCaptures(
    MoveList_t* moveList,
    Bitboard_t freeQueens,
    Bitboard_t hvPinnedQueens,
    Bitboard_t d12PinnedQueens,
    Bitboard_t checkmask,
    Bitboard_t empty,
    Bitboard_t enemyPieces,
    PinmaskContainer_t pinmasks
) 
{
    SerializePositionsIntoMoves(freeQueens, {
        Bitboard_t queenSquare = LSB(freeQueens);
        Bitboard_t moves = QueenCaptureTargets(queenSquare, empty, enemyPieces) & checkmask;
        SerializeNormalMoves(moveList, queenSquare, moves);
    });

    SerializePositionsIntoMoves(hvPinnedQueens, {
        Bitboard_t queenSquare = LSB(hvPinnedQueens);
        Bitboard_t moves = RookCaptureTargets(queenSquare, empty, enemyPieces) & checkmask & pinmasks.hv;
        SerializeNormalMoves(moveList, queenSquare, moves);
    });

    SerializePositionsIntoMoves(d12PinnedQueens, {
        Bitboard_t queenSquare = LSB(d12PinnedQueens);
        Bitboard_t moves = BishopCaptureTargets(queenSquare, empty, enemyPieces) & checkmask & pinmasks.d12;
        SerializeNormalMoves(moveList, queenSquare, moves);
    });
}

static void AddWhitePawnCaptures(
    MoveList_t* moveList,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    Bitboard_t checkmask,
    Bitboard_t empty,
    Bitboard_t enemyPieces,
    PinmaskContainer_t pinmasks
) 
{
    Bitboard_t eastCaptureTargets = 
        (WhiteEastCaptureTargets(freePawns, enemyPieces) |
        (WhiteEastCaptureTargets(d12PinnedPawns, enemyPieces) & pinmasks.d12))
        & checkmask;

    Bitboard_t westCaptureTargets = 
        (WhiteWestCaptureTargets(freePawns, enemyPieces) |
        (WhiteWestCaptureTargets(d12PinnedPawns, enemyPieces) & pinmasks.d12))
        & checkmask;

    Bitboard_t eastEnPassantTargets = 
        (WhiteEastEnPassantTargets(freePawns, ReadEnPassantSquares(white)) |
        WhiteEastEnPassantTargets(d12PinnedPawns, ReadEnPassantSquares(white)) & pinmasks.d12)
        & checkmask;

    Bitboard_t westEnPassantTargets = 
        (WhiteWestEnPassantTargets(freePawns, ReadEnPassantSquares(white)) |
        WhiteWestEnPassantTargets(d12PinnedPawns, ReadEnPassantSquares(white)) & pinmasks.d12)
        & checkmask;
};

void CapturesMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color) {
    moveList->maxIndex = 0;

    UnsafeSquaresCallback_t Callback = UnsafeSquaresCallbacks[color];
    Bitboard_t unsafeSquares = Callback(boardInfo);

    AddKingCaptures(moveList, boardInfo, unsafeSquares, color); 

    Bitboard_t checkmask = full_set;
    if(InCheck(boardInfo->kings[color], unsafeSquares)) {
        checkmask = DefineCheckmask(boardInfo, color);
        if(IsDoubleCheck(boardInfo, checkmask, color)) {
            return;
        }
    } 

    PinmaskContainer_t pinmasks = DefinePinmasks(boardInfo, color);
}

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, Color_t color) {

}