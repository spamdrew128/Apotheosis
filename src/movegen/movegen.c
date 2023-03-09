#include "legals.h"
#include "movegen.h"
#include "pieces.h"
#include "lookup.h"

#define SerializePositionsIntoMoves(_positions, ...) \
    do { \
        while(_positions) { \
            __VA_ARGS__ \
            ResetLSB(&_positions); \
        } \
    } while(0)

static Move_t* CurrentMove(MoveList_t* moveList) {
    return &(moveList->moves[moveList->maxIndex].move);
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
    Bitboard_t knights,
    Bitboard_t filter,
    PinmaskContainer_t pinmasks
)
{
    Bitboard_t freeKnights = knights & ~pinmasks.all;

    SerializePositionsIntoMoves(freeKnights, {
        Bitboard_t knightSquare = LSB(freeKnights);
        Bitboard_t moves = GetKnightAttackSet(knightSquare) & filter; // TODO make this a function
        SerializeNormalMoves(moveList, knightSquare, moves);
    });
}

static void AddD12SliderMoves(
    MoveList_t* moveList,
    Bitboard_t d12Sliders,
    Bitboard_t filter,
    Bitboard_t empty,
    PinmaskContainer_t pinmasks
) 
{
    Bitboard_t freeD12Sliders = d12Sliders & ~pinmasks.all;
    Bitboard_t pinnedD12Sliders = d12Sliders & pinmasks.d12;

    SerializePositionsIntoMoves(freeD12Sliders, {
        Bitboard_t square = LSB(freeD12Sliders);
        Bitboard_t moves = BishopMoveTargets(square, empty, filter);
        SerializeNormalMoves(moveList, square, moves);
    });

    SerializePositionsIntoMoves(pinnedD12Sliders, {
        Bitboard_t square = LSB(pinnedD12Sliders);
        Bitboard_t moves = BishopMoveTargets(square, empty, (filter & pinmasks.d12)) ;
        SerializeNormalMoves(moveList, square, moves);
    });
}

static void AddHvSliderMoves(
    MoveList_t* moveList,
    Bitboard_t hvSliders,
    Bitboard_t filter,
    Bitboard_t empty,
    PinmaskContainer_t pinmasks
) 
{
    Bitboard_t freeHvSliders = hvSliders & ~pinmasks.all;
    Bitboard_t pinnedHvSliders = hvSliders & pinmasks.hv;

    SerializePositionsIntoMoves(freeHvSliders, {
        Bitboard_t square = LSB(freeHvSliders);
        Bitboard_t moves = RookMoveTargets(square, empty, filter);
        SerializeNormalMoves(moveList, square, moves);
    });

    SerializePositionsIntoMoves(pinnedHvSliders, {
        Bitboard_t square = LSB(pinnedHvSliders);
        Bitboard_t moves = RookMoveTargets(square, empty, (filter & pinmasks.hv)) ;
        SerializeNormalMoves(moveList, square, moves);
    });
}

bool EnPassantIsLegal(BoardInfo_t* boardInfo, Bitboard_t toBB, Bitboard_t fromBB, Color_t color) {
    Bitboard_t captureBB = (color == white) ? SoutOne(toBB) : NortOne(toBB);

    ResetBits(&boardInfo->pawns[color], fromBB);
    SetBits(&boardInfo->pawns[color], toBB);
    ResetBits(&boardInfo->pawns[!color], captureBB);

    SetBits(&boardInfo->empty, fromBB|captureBB);
    ResetBits(&boardInfo->empty, toBB);

    Bitboard_t unsafeSquares = UnsafeSquares(boardInfo, color);
    bool isLegal = !InCheck(boardInfo->kings[color], unsafeSquares);

    SetBits(&boardInfo->pawns[color], fromBB);
    ResetBits(&boardInfo->pawns[color], toBB);
    SetBits(&boardInfo->pawns[!color], captureBB);

    ResetBits(&boardInfo->empty, fromBB|captureBB);
    SetBits(&boardInfo->empty, toBB);

    return isLegal;
}

static void AddEnPassantMove(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t toBB,
    Bitboard_t fromBB
)
{
    InitializeNewMove(moveList);
    Move_t* current = CurrentMove(moveList);

    WriteToSquare(current, LSB(toBB));
    WriteFromSquare(current, LSB(fromBB));
    WriteSpecialFlag(current, en_passant_flag);
}

static void AddWhitePawnCaptures(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    GameStack_t* gameStack,
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

    Bitboard_t enPassantBB = ReadEnPassant(gameStack);
    if(CanEastEnPassant(gameStack)) {
        AddEnPassantMove(moveList, boardInfo, enPassantBB, SoWeOne(enPassantBB));
    }
    if(CanWestEnPassant(gameStack)) {
        AddEnPassantMove(moveList, boardInfo, enPassantBB, SoEaOne(enPassantBB));
    }
};

static void AddBlackPawnCaptures(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t freePawns,
    Bitboard_t d12PinnedPawns,
    GameStack_t* gameStack,
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

    Bitboard_t enPassantBB = ReadEnPassant(gameStack);
    if(CanEastEnPassant(gameStack)) {
        AddEnPassantMove(moveList, boardInfo, enPassantBB, NoWeOne(enPassantBB));
    }
    if(CanWestEnPassant(gameStack)) {
        AddEnPassantMove(moveList, boardInfo, enPassantBB, NoEaOne(enPassantBB));
    }
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

static void AddKnightAndSliderMoves(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t checkmask,
    Bitboard_t filter,
    PinmaskContainer_t pinmasks,
    Color_t color
)
{
    AddKnightMoves(
        moveList,
        boardInfo->knights[color],
        filter,
        pinmasks
    );

    AddD12SliderMoves(
        moveList,
        AllD12Sliders(boardInfo, color),
        filter,
        boardInfo->empty,
        pinmasks
    );

    AddHvSliderMoves(
        moveList,
        AllHvSliders(boardInfo, color),
        filter,
        boardInfo->empty,
        pinmasks 
    );
}

static void AddAllCaptures(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t checkmask,
    GameStack_t* gameStack,
    PinmaskContainer_t pinmasks,
    Color_t color
)
{
    Bitboard_t enemyPieces = boardInfo->allPieces[!color];
    if(color == white) {
        AddWhitePawnCaptures(
            moveList,
            boardInfo,
            boardInfo->pawns[color] & ~pinmasks.all,
            boardInfo->pawns[color] & pinmasks.d12,
            gameStack,
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
            gameStack,
            enemyPieces,
            checkmask,
            pinmasks        
        );        
    }

    Bitboard_t filter = checkmask & boardInfo->allPieces[!color];
    AddKnightAndSliderMoves(
        moveList,
        boardInfo,
        checkmask,
        filter,
        pinmasks,
        color
    );
}

static void AddAllQuietMoves(
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Bitboard_t checkmask,
    PinmaskContainer_t pinmasks,
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

    Bitboard_t filter = checkmask & boardInfo->empty;
    AddKnightAndSliderMoves(
        moveList,
        boardInfo,
        checkmask,
        filter,
        pinmasks,
        color
    );
}

void CompleteMovegen(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack) {
    moveList->maxIndex = movelist_empty;

    Color_t color = boardInfo->colorToMove;

    Bitboard_t unsafeSquares = UnsafeSquares(boardInfo, color);
    Square_t kingSquare = KingSquare(boardInfo, color);
    Bitboard_t enemyPieces = boardInfo->allPieces[!color];

    AddKingMoves(
        moveList,
        kingSquare,
        KingMoveTargets(kingSquare, enemyPieces),
        unsafeSquares,
        boardInfo->empty
    );

    Bitboard_t checkmask = full_set;
    bool inCheck = InCheck(boardInfo->kings[color], unsafeSquares);
    if(inCheck) {
        checkmask = DefineCheckmask(boardInfo, color);
        if(IsDoubleCheck(boardInfo, checkmask, color)) {
            moveList->maxCapturesIndex = moveList->maxIndex;

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
        checkmask,
        stack,
        pinmasks,
        color
    );

    moveList->maxCapturesIndex = moveList->maxIndex;

    AddAllQuietMoves(
        moveList,
        boardInfo,
        checkmask,
        pinmasks,
        color
    );

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