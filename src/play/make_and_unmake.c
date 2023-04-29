#include <stdbool.h>
#include <assert.h>

#include "make_and_unmake.h"
#include "movegen.h"
#include "game_state.h"
#include "lookup.h"

#define PAWN_START_RANKS (RANK_2 | RANK_7)
#define PAWN_DOUBLE_RANKS (RANK_4 | RANK_5)

static bool PawnIsDoublePushed(Bitboard_t fromBB, Bitboard_t toBB) {
    return (fromBB & PAWN_START_RANKS) && (toBB & PAWN_DOUBLE_RANKS);
}

static Bitboard_t GetEnPassantBB(Bitboard_t toBB, Color_t color) {
    if(color == white) {
        return SoutOne(toBB);
    } else {
        return NortOne(toBB);
    }
}

static void UpdateEnPassantInfo(BoardInfo_t* info, GameState_t* nextState, Bitboard_t fromBB, Bitboard_t toBB, Color_t color) {
    Bitboard_t eastAdjPawn = info->pawns[!color] & EastOne(toBB);
    Bitboard_t westAdjPawn = info->pawns[!color] & WestOne(toBB);
    Bitboard_t enPassantSquare = GetEnPassantBB(toBB, color);

    nextState->canWestEP = eastAdjPawn && EnPassantIsLegal(info, enPassantSquare, eastAdjPawn, !color);
    nextState->canEastEP = westAdjPawn && EnPassantIsLegal(info, enPassantSquare, westAdjPawn, !color);
    nextState->enPassantSquare = enPassantSquare;
}

static void UpdateCastleSquares(GameState_t* nextState, BoardInfo_t* info, Color_t color) {
    Bitboard_t rooksInPlace = BOARD_CORNERS & info->rooks[color];
    Bitboard_t validCastlingMask = GenShiftWest(rooksInPlace, 1) | GenShiftEast(rooksInPlace, 2);
    
    nextState->castleSquares[color] &= validCastlingMask;
}

static void RemoveCapturedPiece(
    BoardInfo_t* boardInfo,
    Square_t capturedSquare,
    Piece_t type,
    Color_t capturedPieceColor
) 
{
    Bitboard_t capturedBB = GetSingleBitset(capturedSquare);

    ResetBits(GetPieceInfoField(boardInfo, type, capturedPieceColor), capturedBB);

    ResetBits(&(boardInfo->allPieces[capturedPieceColor]), capturedBB);
}

static void RemoveCapturedEnPassant(
    BoardInfo_t* boardInfo,
    Bitboard_t enPassantBB,
    Color_t capturedPieceColor
)
{
    ResetBits(&(boardInfo->pawns[capturedPieceColor]), enPassantBB);
    ResetBits(&(boardInfo->allPieces[capturedPieceColor]), enPassantBB);
    RemovePieceFromMailbox(boardInfo, LSB(enPassantBB));
}

static void UpdateBoardInfoField(
    BoardInfo_t* boardInfo,
    Bitboard_t* infoField,
    Bitboard_t fromBB,
    Bitboard_t toBB,
    Square_t fromSquare,
    Square_t toSquare,
    Color_t color
) 
{
    ResetBits(infoField, fromBB);
    SetBits(infoField, toBB);

    ResetBits(&(boardInfo->allPieces[color]), fromBB);
    SetBits(&(boardInfo->allPieces[color]), toBB);

    MovePieceInMailbox(boardInfo, toSquare, fromSquare);
}

static void MakeCastlingHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move) {
    Color_t color = boardInfo->colorToMove;
    Square_t kingFromSquare = ReadFromSquare(move);
    Square_t kingToSquare = ReadToSquare(move);

    Bitboard_t kingFromBB = boardInfo->kings[color];
    Bitboard_t kingToBB = GetSingleBitset(kingToSquare);

    UpdateBoardInfoField(
        boardInfo,
        &(boardInfo->kings[color]),
        kingFromBB,
        kingToBB,
        kingFromSquare,
        kingToSquare,
        color
    );

    if(kingToSquare < kingFromSquare) { // queenside castle
        Bitboard_t rookFromBB = GenShiftWest(kingFromBB, 4);
        Bitboard_t rookToBB = GenShiftWest(kingFromBB, 1);
        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->rooks[color]),
            rookFromBB,
            rookToBB,
            LSB(rookFromBB),
            LSB(rookToBB),
            color
        );

    } else {
        Bitboard_t rookFromBB = GenShiftEast(kingFromBB, 3);
        Bitboard_t rookToBB = GenShiftEast(kingFromBB, 1);
        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->rooks[color]),
            rookFromBB,
            rookToBB,
            LSB(rookFromBB),
            LSB(rookToBB),
            color
        );
    }

    UpdateEmpty(boardInfo);
    nextState->castleSquares[color] = EMPTY_SET;
}

static void MakePromotionHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move) {
    Color_t color = boardInfo->colorToMove;
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);
    Piece_t promotionPiece = ReadPromotionPiece(move);

    Piece_t capturedPiece = PieceOnSquare(boardInfo, toSquare);
    if(capturedPiece != none_type) {
        RemoveCapturedPiece(
            boardInfo,
            toSquare,
            capturedPiece,
            !color
        );

        nextState->capturedPiece = capturedPiece;
        UpdateCastleSquares(nextState, boardInfo, !color);
    }

    AddPieceToMailbox(boardInfo, fromSquare, promotionPiece);
    MovePieceInMailbox(boardInfo, toSquare, fromSquare);

    Bitboard_t fromBB = GetSingleBitset(fromSquare);
    Bitboard_t toBB = GetSingleBitset(toSquare);

    ResetBits(&(boardInfo->pawns[color]), fromBB);
    ResetBits(&(boardInfo->allPieces[color]), fromBB);
    SetBits(&(boardInfo->allPieces[color]), toBB);

    SetBits(GetPieceInfoField(boardInfo, promotionPiece, color), toBB);

    UpdateEmpty(boardInfo);

    nextState->halfmoveClock = EMPTY_SET;
}

static void MakeEnPassantHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move) {
    Color_t color = boardInfo->colorToMove;
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);
    Bitboard_t fromBB = GetSingleBitset(fromSquare);
    Bitboard_t toBB = GetSingleBitset(toSquare);
    Bitboard_t enPassantBB = GetEnPassantBB(toBB, color);

    RemoveCapturedEnPassant(
        boardInfo,
        enPassantBB,
        !color
    );

    UpdateBoardInfoField(
        boardInfo,
        &(boardInfo->pawns[color]),
        fromBB,
        toBB,
        fromSquare,
        toSquare,
        color
    );
    UpdateEmpty(boardInfo);

    nextState->halfmoveClock = EMPTY_SET;
    nextState->enPassantSquare = EMPTY_SET;
    nextState->capturedPiece = pawn;
}

static void MakeMoveDefaultHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move) {
    Color_t color = boardInfo->colorToMove;
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);
    Bitboard_t fromBB = GetSingleBitset(fromSquare);
    Bitboard_t toBB = GetSingleBitset(toSquare);

    Piece_t capturedPiece = PieceOnSquare(boardInfo, toSquare);
    if(capturedPiece != none_type) {
        RemoveCapturedPiece(
            boardInfo,
            toSquare,
            capturedPiece,
            !color
        );

        nextState->halfmoveClock = 0;
        nextState->capturedPiece = capturedPiece;
        UpdateCastleSquares(nextState, boardInfo, !color); // if we captured, we might have messed up our opponent's castling rights
    }

    bool pawnDoublePushed = false;
    Piece_t type = PieceOnSquare(boardInfo, fromSquare);
    switch (type) {
        case pawn:
            nextState->halfmoveClock = 0;
            pawnDoublePushed = PawnIsDoublePushed(fromBB, toBB);
        break;
        case king:
            nextState->castleSquares[color] = EMPTY_SET;
        break;
    }

    UpdateBoardInfoField(
        boardInfo,
        GetPieceInfoField(boardInfo, type, color),
        fromBB,
        toBB,
        fromSquare,
        toSquare,
        color
    );

    UpdateCastleSquares(nextState, boardInfo, color);
    UpdateEmpty(boardInfo);

    if(pawnDoublePushed) {
        UpdateEnPassantInfo(boardInfo, nextState, fromBB, toBB, color);
    }
}

void MakeMove(BoardInfo_t* boardInfo, GameStack_t* gameStack, Move_t move) {
    SpecialFlag_t specialFlag = ReadSpecialFlag(move);
    GameState_t* nextState = GetDefaultNextGameState(gameStack);
    
    switch (specialFlag) {
        case castle_flag:
            MakeCastlingHandler(boardInfo, nextState, move);
        break;
        case promotion_flag:
            MakePromotionHandler(boardInfo, nextState, move);
        break;
        case en_passant_flag:
            MakeEnPassantHandler(boardInfo, nextState, move);
        break;
        default:
            MakeMoveDefaultHandler(boardInfo, nextState, move);
        break;
    }

    boardInfo->colorToMove = !(boardInfo->colorToMove);
    nextState->boardInfo = *boardInfo;
}

void UnmakeMove(BoardInfo_t* boardInfo, GameStack_t* gameStack) {
    RevertState(gameStack);
    *boardInfo = ReadCurrentBoardInfo(gameStack);
}

void MakeNullMove(BoardInfo_t* boardInfo, GameStack_t* gameStack, ZobristStack_t* zobristStack) {
    GameState_t* nextState = GetDefaultNextGameState(gameStack);
    boardInfo->colorToMove = !(boardInfo->colorToMove);
    nextState->boardInfo = *boardInfo;

    AddZobristHashToStack(zobristStack, HashPosition(boardInfo, gameStack));
}