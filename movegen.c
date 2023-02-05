#include "legals.h"
#include "movegen.h"
#include "pieces.h"
#include "lookup.h"

enum {
    movelist_empty = -1
};

typedef Bitboard_t (*UnsafeSquaresCallback_t)(BoardInfo_t* boardInfo);

typedef void (*AddPawnCaptures_t)(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    Bitboard_t enemyPieces,
    Bitboard_t checkmask,
    PinmaskContainer_t pinmasks
);

typedef void (*AddPawnMoves_t)(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t hvPinnedPawns,
    Bitboard_t empty,
    Bitboard_t checkmask,
    PinmaskContainer_t pinmasks
);

typedef Bitboard_t (*SliderCaptureTargetsCallback_t)(Square_t square, Bitboard_t empty, Bitboard_t enemyPieces);

#define SerializePositionsIntoMoves(_positions, ...) \
    do { \
        while(_positions) { \
            __VA_ARGS__ \
            ResetLSB(&_positions); \
        } \
    } while(0)

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

static void SerializePawnMoves(
    MoveList_t* moveList,
    Bitboard_t moves,
    SpecialFlag_t flag,
    DirectionCallback_t ShiftToPawnPos
)
{
    Bitboard_t pawnPositions = ShiftToPawnPos(moves);
    while(moves) {
        InitializeNewMove(moveList);
        Move_t* current = CurrentMove(moveList);

        WriteToSquare(current, LSB(moves));
        WriteFromSquare(current, LSB(pawnPositions));
        WriteSpecialFlag(current, flag);

        ResetLSB(&moves);
        ResetLSB(&pawnPositions);
    }
}

static void _SerializePawnPromotionsHelper(MoveList_t* moveList, Piece_t promotionType, Square_t to, Square_t from) {
    InitializeNewMove(moveList);
    Move_t* current = CurrentMove(moveList);

    WriteToSquare(current, to);
    WriteFromSquare(current, from);
    WritePromotionPiece(current, promotionType);
    WriteSpecialFlag(current, promotion_flag);
}

static void SerializePawnPromotions(
    MoveList_t* moveList,
    Bitboard_t moves,
    DirectionCallback_t ShiftToPawnPos
)
{
    Bitboard_t pawnPositions = ShiftToPawnPos(moves);
    while(moves) {
        Square_t toSquare = LSB(moves);
        Square_t fromSquare = LSB(pawnPositions);

        _SerializePawnPromotionsHelper(moveList, queen, toSquare, fromSquare);
        _SerializePawnPromotionsHelper(moveList, rook, toSquare, fromSquare);
        _SerializePawnPromotionsHelper(moveList, bishop, toSquare, fromSquare);
        _SerializePawnPromotionsHelper(moveList, knight, toSquare, fromSquare);

        ResetLSB(&moves);
        ResetLSB(&pawnPositions);
    }
}

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
    GameStack_t* stack,
    Bitboard_t unsafeSquares,
    Square_t kingSquare,
    Color_t color
)
{
    if(CanCastleKingside(boardInfo, unsafeSquares, ReadCastleSquares(stack, color), color)) {
        _AddCastlingMovesHelper(moveList, kingSquare, GetKingsideCastleSquare(color));
    }
    
    if(CanCastleQueenside(boardInfo, unsafeSquares, ReadCastleSquares(stack, color), color)) {
        _AddCastlingMovesHelper(moveList, kingSquare, GetQueensideCastleSquare(color));
    }
}

static void AddKnightMoves(
    MoveList_t* moveList,
    Bitboard_t freeKnights,
    Bitboard_t empty,
    Bitboard_t checkmask,
    Color_t color 
) 
{
    SerializePositionsIntoMoves(freeKnights, {
        Bitboard_t knightSquare = LSB(freeKnights);
        Bitboard_t moves = KnightMoveTargets(knightSquare, empty) & checkmask;
        SerializeNormalMoves(moveList, knightSquare, moves);
    });
}

static void AddKnightCaptures(
    MoveList_t* moveList,
    Bitboard_t freeKnights,
    Bitboard_t enemyPieces,
    Bitboard_t checkmask,
    Color_t color 
) 
{
    SerializePositionsIntoMoves(freeKnights, {
        Bitboard_t knightSquare = LSB(freeKnights);
        Bitboard_t moves = KnightCaptureTargets(knightSquare, enemyPieces) & checkmask;
        SerializeNormalMoves(moveList, knightSquare, moves);
    });
}

static void AddBishopMoves(
    MoveList_t* moveList,
    Bitboard_t freeBishops,
    Bitboard_t d12PinnedBishops,
    Bitboard_t checkmask,
    Bitboard_t empty,
    PinmaskContainer_t pinmasks
) 
{
    SerializePositionsIntoMoves(freeBishops, {
        Bitboard_t bishopSquare = LSB(freeBishops);
        Bitboard_t moves = BishopMoveTargets(bishopSquare, empty) & checkmask;
        SerializeNormalMoves(moveList, bishopSquare, moves);
    });

    SerializePositionsIntoMoves(d12PinnedBishops, {
        Bitboard_t bishopSquare = LSB(d12PinnedBishops);
        Bitboard_t moves = BishopMoveTargets(bishopSquare, empty) & checkmask & pinmasks.d12;
        SerializeNormalMoves(moveList, bishopSquare, moves);
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

static void AddRookMoves(
    MoveList_t* moveList,
    Bitboard_t freeRooks,
    Bitboard_t hvPinnedRooks,
    Bitboard_t checkmask,
    Bitboard_t empty,
    PinmaskContainer_t pinmasks
) 
{
    SerializePositionsIntoMoves(freeRooks, {
        Bitboard_t rookSquare = LSB(freeRooks);
        Bitboard_t moves = RookMoveTargets(rookSquare, empty) & checkmask;
        SerializeNormalMoves(moveList, rookSquare, moves);
    });

    SerializePositionsIntoMoves(hvPinnedRooks, {
        Bitboard_t rookSquare = LSB(hvPinnedRooks);
        Bitboard_t moves = RookMoveTargets(rookSquare, empty) & checkmask & pinmasks.hv;
        SerializeNormalMoves(moveList, rookSquare, moves);
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

static void AddQueenMoves(
    MoveList_t* moveList,
    Bitboard_t freeQueens,
    Bitboard_t hvPinnedQueens,
    Bitboard_t d12PinnedQueens,
    Bitboard_t checkmask,
    Bitboard_t empty,
    PinmaskContainer_t pinmasks
) 
{
    SerializePositionsIntoMoves(freeQueens, {
        Bitboard_t queenSquare = LSB(freeQueens);
        Bitboard_t moves = QueenMoveTargets(queenSquare, empty) & checkmask;
        SerializeNormalMoves(moveList, queenSquare, moves);
    });

    SerializePositionsIntoMoves(hvPinnedQueens, {
        Bitboard_t queenSquare = LSB(hvPinnedQueens);
        Bitboard_t moves = RookMoveTargets(queenSquare, empty) & checkmask & pinmasks.hv;
        SerializeNormalMoves(moveList, queenSquare, moves);
    });

    SerializePositionsIntoMoves(d12PinnedQueens, {
        Bitboard_t queenSquare = LSB(d12PinnedQueens);
        Bitboard_t moves = BishopMoveTargets(queenSquare, empty) & checkmask & pinmasks.d12;
        SerializeNormalMoves(moveList, queenSquare, moves);
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

static void TryWhiteEnPassant(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t toBB,
    Bitboard_t fromBB,
    Color_t color
)
{
    Bitboard_t captureBB = SoutOne(toBB);

    ResetBits(&boardInfo->pawns[color], fromBB);
    SetBits(&boardInfo->pawns[color], toBB);
    ResetBits(&boardInfo->pawns[!color], captureBB);

    Bitboard_t unsafeSquares = WhiteUnsafeSquares(boardInfo);
    if(!InCheck(boardInfo->kings[color], unsafeSquares)) {
        InitializeNewMove(moveList);
        Move_t* current = CurrentMove(moveList);

        WriteToSquare(current, LSB(toBB));
        WriteFromSquare(current, LSB(fromBB));
        WriteSpecialFlag(current, en_passant_flag);
    }

    SetBits(&boardInfo->pawns[color], fromBB);
    ResetBits(&boardInfo->pawns[color], toBB);
    SetBits(&boardInfo->pawns[!color], captureBB);
}

static void TryBlackEnPassant(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t toBB,
    Bitboard_t fromBB,
    Color_t color
)
{
    Bitboard_t captureBB = NortOne(toBB);

    ResetBits(&boardInfo->pawns[color], fromBB);
    SetBits(&boardInfo->pawns[color], toBB);
    ResetBits(&boardInfo->pawns[!color], captureBB);

    Bitboard_t unsafeSquares = BlackUnsafeSquares(boardInfo);
    if(!InCheck(boardInfo->kings[color], unsafeSquares)) {
        InitializeNewMove(moveList);
        Move_t* current = CurrentMove(moveList);

        WriteToSquare(current, LSB(toBB));
        WriteFromSquare(current, LSB(fromBB));
        WriteSpecialFlag(current, en_passant_flag);
    }

    SetBits(&boardInfo->pawns[color], fromBB);
    ResetBits(&boardInfo->pawns[color], toBB);
    SetBits(&boardInfo->pawns[!color], captureBB);
}

static void AddWhiteLegalEnPassant(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    Bitboard_t enPassantBB,
    PinmaskContainer_t pinmasks
)
{
    Bitboard_t eastLegalEnPassantTargets = 
        WhiteEastEnPassantTargets(freePawns, enPassantBB) |
        (WhiteEastEnPassantTargets(d12PinnedPawns, enPassantBB) & pinmasks.d12);

    Bitboard_t westLegalEnPassantTargets = 
        WhiteWestEnPassantTargets(freePawns, enPassantBB) |
        (WhiteWestEnPassantTargets(d12PinnedPawns, enPassantBB) & pinmasks.d12);

    if(eastLegalEnPassantTargets) {
        TryWhiteEnPassant(
            moveList,
            boardInfo,
            eastLegalEnPassantTargets,
            SoWeOne(eastLegalEnPassantTargets),
            white
        );
    }

    if(westLegalEnPassantTargets) {
        TryWhiteEnPassant(
            moveList,
            boardInfo,
            westLegalEnPassantTargets,
            SoEaOne(westLegalEnPassantTargets),
            white
        );
    }
}

static void AddBlackLegalEnPassant(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    Bitboard_t enPassantBB,
    PinmaskContainer_t pinmasks
)
{
    Bitboard_t eastLegalEnPassantTargets = 
        BlackEastEnPassantTargets(freePawns, enPassantBB) |
        (BlackEastEnPassantTargets(d12PinnedPawns, enPassantBB) & pinmasks.d12);

    Bitboard_t westLegalEnPassantTargets = 
        BlackWestEnPassantTargets(freePawns, enPassantBB) |
        (BlackWestEnPassantTargets(d12PinnedPawns, enPassantBB) & pinmasks.d12);

    if(eastLegalEnPassantTargets) {
        TryBlackEnPassant(
            moveList,
            boardInfo,
            eastLegalEnPassantTargets,
            NoWeOne(eastLegalEnPassantTargets),
            black
        );
    }

    if(westLegalEnPassantTargets) {
        TryBlackEnPassant(
            moveList,
            boardInfo,
            westLegalEnPassantTargets,
            NoEaOne(westLegalEnPassantTargets),
            black
        );
    }
}

static void AddWhitePawnCaptures(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    Bitboard_t enPassantBB,
    Bitboard_t enemyPieces,
    Bitboard_t checkmask,
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

    Bitboard_t eastCapturePromotions = FilterWhitePromotions(&eastCaptureTargets);
    Bitboard_t westCapturePromotions = FilterWhitePromotions(&westCaptureTargets);

    SerializePawnMoves(moveList, eastCaptureTargets, no_flag, SoWeOne);
    SerializePawnPromotions(moveList, eastCapturePromotions, SoWeOne);

    SerializePawnMoves(moveList, westCaptureTargets, no_flag, SoEaOne);
    SerializePawnPromotions(moveList, westCapturePromotions, SoEaOne);

    AddWhiteLegalEnPassant(
        moveList,
        boardInfo,
        freePawns,
        d12PinnedPawns,
        enPassantBB,
        pinmasks
    );
};

static void AddBlackPawnCaptures(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    Bitboard_t enPassantBB,
    Bitboard_t enemyPieces,
    Bitboard_t checkmask,
    PinmaskContainer_t pinmasks
) 
{
    Bitboard_t eastCaptureTargets = 
        (BlackEastCaptureTargets(freePawns, enemyPieces) |
        (BlackEastCaptureTargets(d12PinnedPawns, enemyPieces) & pinmasks.d12))
        & checkmask;

    Bitboard_t westCaptureTargets = 
        (BlackWestCaptureTargets(freePawns, enemyPieces) |
        (BlackWestCaptureTargets(d12PinnedPawns, enemyPieces) & pinmasks.d12))
        & checkmask;

    Bitboard_t eastCapturePromotions = FilterBlackPromotions(&eastCaptureTargets);
    Bitboard_t westCapturePromotions = FilterBlackPromotions(&westCaptureTargets);

    SerializePawnMoves(moveList, eastCaptureTargets, no_flag, NoWeOne);
    SerializePawnPromotions(moveList, eastCapturePromotions, NoWeOne);

    SerializePawnMoves(moveList, westCaptureTargets, no_flag, NoEaOne);
    SerializePawnPromotions(moveList, westCapturePromotions, NoEaOne);

    AddBlackLegalEnPassant(
        moveList,
        boardInfo,
        freePawns,
        d12PinnedPawns,
        enPassantBB,
        pinmasks
    );
};

static void AddWhitePawnMoves(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t hvPinnedPawns,
    Bitboard_t empty,
    Bitboard_t checkmask,
    PinmaskContainer_t pinmasks
)
{
    Bitboard_t singleMoveTargets = 
        (WhiteSinglePushTargets(freePawns, empty) |
        (WhiteSinglePushTargets(hvPinnedPawns, empty) & pinmasks.hv))
        & checkmask;

    Bitboard_t doubleMoveTargets = 
        (WhiteDoublePushTargets(freePawns, empty) |
        (WhiteDoublePushTargets(hvPinnedPawns, empty) & pinmasks.hv))
        & checkmask;

    Bitboard_t singleMovePromotions = FilterWhitePromotions(&singleMoveTargets);
    Bitboard_t doubleMovePromotions = FilterWhitePromotions(&doubleMoveTargets);

    SerializePawnMoves(moveList, singleMoveTargets, no_flag, SoutOne);
    SerializePawnPromotions(moveList, singleMovePromotions, SoutOne);

    SerializePawnMoves(moveList, doubleMoveTargets, no_flag, SoutTwo);
    SerializePawnPromotions(moveList, doubleMovePromotions, SoutTwo);
};

static void AddBlackPawnMoves(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t hvPinnedPawns,
    Bitboard_t empty,
    Bitboard_t checkmask,
    PinmaskContainer_t pinmasks
)
{
    Bitboard_t singleMoveTargets = 
        (BlackSinglePushTargets(freePawns, empty) |
        (BlackSinglePushTargets(hvPinnedPawns, empty) & pinmasks.hv))
        & checkmask;

    Bitboard_t doubleMoveTargets = 
        (BlackDoublePushTargets(freePawns, empty) |
        (BlackDoublePushTargets(hvPinnedPawns, empty) & pinmasks.hv))
        & checkmask;

    Bitboard_t singleMovePromotions = FilterBlackPromotions(&singleMoveTargets);
    Bitboard_t doubleMovePromotions = FilterBlackPromotions(&doubleMoveTargets);

    SerializePawnMoves(moveList, singleMoveTargets, no_flag, NortOne);
    SerializePawnPromotions(moveList, singleMovePromotions, NortOne);

    SerializePawnMoves(moveList, doubleMoveTargets, no_flag, NortTwo);
    SerializePawnPromotions(moveList, doubleMovePromotions, NortTwo);
};

static void AddAllCaptures(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    PinmaskContainer_t pinmasks,
    Bitboard_t enemyPieces,
    Bitboard_t enPassantBB,
    Bitboard_t checkmask,
    Color_t color
)
{
    if(color == white) {
        AddWhitePawnCaptures(
            moveList,
            boardInfo,
            boardInfo->pawns[color] & ~pinmasks.all,
            boardInfo->pawns[color] & pinmasks.d12,
            enPassantBB,
            enemyPieces,
            checkmask,
            pinmasks        
        );
    } else {
        AddBlackPawnCaptures(
            moveList,
            boardInfo,
            boardInfo->pawns[color] & ~pinmasks.all,
            boardInfo->pawns[color] & pinmasks.d12,
            enPassantBB,
            enemyPieces,
            checkmask,
            pinmasks        
        );        
    }

    AddKnightCaptures(
        moveList,
        boardInfo->knights[color] & ~pinmasks.all,
        enemyPieces,
        checkmask,
        color
    );

    AddBishopCaptures(
        moveList,
        boardInfo->bishops[color] & ~pinmasks.all,
        boardInfo->bishops[color] & pinmasks.d12,
        checkmask,
        boardInfo->empty,
        enemyPieces,
        pinmasks
    );

    AddRookCaptures(
        moveList,
        boardInfo->rooks[color] & ~pinmasks.all,
        boardInfo->rooks[color] & pinmasks.hv,
        checkmask,
        boardInfo->empty,
        enemyPieces,
        pinmasks
    );

    AddQueenCaptures(
        moveList,
        boardInfo->queens[color] & ~pinmasks.all,
        boardInfo->queens[color] & pinmasks.hv,
        boardInfo->queens[color] & pinmasks.d12,
        checkmask,
        boardInfo->empty,
        enemyPieces,
        pinmasks
    );
}

static void AddAllQuietMoves(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    PinmaskContainer_t pinmasks,
    Bitboard_t checkmask,
    Color_t color
)
{
    if(color == white) {
        AddWhitePawnMoves(
            moveList,
            boardInfo,
            boardInfo->pawns[color] & ~pinmasks.all,
            boardInfo->pawns[color] & pinmasks.hv,
            boardInfo->empty,
            checkmask,
            pinmasks
        );
    } else {
        AddBlackPawnMoves(
            moveList,
            boardInfo,
            boardInfo->pawns[color] & ~pinmasks.all,
            boardInfo->pawns[color] & pinmasks.hv,
            boardInfo->empty,
            checkmask,
            pinmasks
        );
    }

    AddKnightMoves(
        moveList,
        boardInfo->knights[color] & ~pinmasks.all,
        boardInfo->empty,
        checkmask,
        color
    );

    AddBishopMoves(
        moveList,
        boardInfo->bishops[color] & ~pinmasks.all,
        boardInfo->bishops[color] & pinmasks.d12,
        checkmask,
        boardInfo->empty,
        pinmasks
    );

    AddRookMoves(
        moveList,
        boardInfo->rooks[color] & ~pinmasks.all,
        boardInfo->rooks[color] & pinmasks.hv,
        checkmask,
        boardInfo->empty,
        pinmasks
    );

    AddQueenMoves(
        moveList,
        boardInfo->queens[color] & ~pinmasks.all,
        boardInfo->queens[color] & pinmasks.hv,
        boardInfo->queens[color] & pinmasks.d12,
        checkmask,
        boardInfo->empty,
        pinmasks
    );
}

void CapturesMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack, Color_t color) {
    moveList->maxIndex = movelist_empty;

    Bitboard_t unsafeSquares;
    if(color == white) {
        unsafeSquares = WhiteUnsafeSquares(boardInfo);
    } else {
        unsafeSquares = BlackUnsafeSquares(boardInfo);
    }

    Square_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t enemyPieces = boardInfo->allPieces[!color];

    AddKingMoves(
        moveList,
        kingSquare,
        KingCaptureTargets(kingSquare, enemyPieces),
        unsafeSquares,
        boardInfo->empty
    ); 

    Bitboard_t checkmask = full_set;
    if(InCheck(boardInfo->kings[color], unsafeSquares)) {
        checkmask = DefineCheckmask(boardInfo, color);
        if(IsDoubleCheck(boardInfo, checkmask, color)) {
            return;
        }
    } 

    PinmaskContainer_t pinmasks = DefinePinmasks(boardInfo, color);

    AddAllCaptures(
        moveList,
        boardInfo,
        pinmasks,
        enemyPieces,
        ReadEnPassantSquares(stack),
        checkmask,
        color
    );
}

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack, Color_t color) {
    moveList->maxIndex = movelist_empty;

    Bitboard_t unsafeSquares;
    if(color == white) {
        unsafeSquares = WhiteUnsafeSquares(boardInfo);
    } else {
        unsafeSquares = BlackUnsafeSquares(boardInfo);
    }

    Square_t kingSquare = LSB(boardInfo->kings[color]);
    Bitboard_t enemyPieces = boardInfo->allPieces[!color];

    AddKingMoves(
        moveList,
        kingSquare,
        KingCaptureTargets(kingSquare, enemyPieces),
        unsafeSquares,
        boardInfo->empty
    );

    Bitboard_t checkmask = full_set;
    bool inCheck = InCheck(boardInfo->kings[color], unsafeSquares);
    if(inCheck) {
        checkmask = DefineCheckmask(boardInfo, color);
        if(IsDoubleCheck(boardInfo, checkmask, color)) {
            AddKingMoves(
                moveList,
                kingSquare,
                KingMoveTargets(kingSquare, boardInfo->empty),
                unsafeSquares,
                boardInfo->empty
            );
            return;
        }
    }

    PinmaskContainer_t pinmasks = DefinePinmasks(boardInfo, color);

    AddAllCaptures(
        moveList,
        boardInfo,
        pinmasks,
        enemyPieces,
        ReadEnPassantSquares(stack),
        checkmask,
        color
    );

    AddAllQuietMoves(
        moveList,
        boardInfo,
        pinmasks,
        checkmask,
        color
    );

    // kingmoves here for ordering purposes
    AddKingMoves(
        moveList,
        kingSquare,
        KingMoveTargets(kingSquare, boardInfo->empty),
        unsafeSquares,
        boardInfo->empty
    );

    if(!inCheck) {
        AddCastlingMoves(
            moveList,
            boardInfo,
            stack,
            unsafeSquares,
            kingSquare,
            color
        );
    }
}