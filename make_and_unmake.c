#include <stdbool.h>
#include <assert.h>

#include "make_and_unmake.h"
#include "game_state.h"
#include "lookup.h"

enum {
    pawn_start_ranks = rank_2 | rank_7,
    pawn_double_ranks = rank_4 | rank_5
};

static Bitboard_t kingsideRooks[] = { C64(1) << h1, C64(1) << h8 };
static Bitboard_t queensideRooks[] = { C64(1) << a1, C64(1) << a8 };

static void UpdatecastleRights(CastleRights_t* castleRights, BoardInfo_t* info, Color_t color) {
    bool validKingsideRook = kingsideRooks[color] & info->rooks[color];
    bool validQueensideRook = queensideRooks[color] & info->rooks[color];

    if(!validKingsideRook) {
        ResetKingsideCastleRights(castleRights, color);
    }
    if(!validQueensideRook) {
        ResetQueensideCastleRights(castleRights, color);
    }
}

static bool PawnIsDoublePushed(Bitboard_t fromBB, Bitboard_t toBB) {
    return (fromBB & pawn_start_ranks) && (toBB & pawn_double_ranks);
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

static void MakeCastlingHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move, Color_t color) {
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

    ResetAllRights(&nextState->castleRights, color);
}

static void MakePromotionHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move, Color_t color) {
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
        UpdatecastleRights(&nextState->castleRights, boardInfo, !color);
    }

    AddPieceToMailbox(boardInfo, fromSquare, promotionPiece);
    MovePieceInMailbox(boardInfo, toSquare, fromSquare);

    Bitboard_t fromBB = GetSingleBitset(fromSquare);
    Bitboard_t toBB = GetSingleBitset(toSquare);

    ResetBits(&(boardInfo->pawns[color]), fromBB);
    ResetBits(&(boardInfo->allPieces[color]), fromBB);
    SetBits(&(boardInfo->allPieces[color]), toBB);

    SetBits(GetPieceInfoField(boardInfo, promotionPiece, color), toBB);

    nextState->halfmoveClock = empty_set;
}

static Bitboard_t GetEnPassantBB(Bitboard_t toBB, Color_t color) {
    if(color == white) {
        return SoutOne(toBB);
    } else {
        return NortOne(toBB);
    }
}

static void MakeEnPassantHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move, Color_t color) {
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

    nextState->halfmoveClock = empty_set;
    nextState->enPassantSquares = empty_set;
    nextState->capturedPiece = pawn;
}

static void MakeMoveDefaultHandler(BoardInfo_t* boardInfo, GameState_t* nextState, Move_t move, Color_t color) {
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
        UpdatecastleRights(&nextState->castleRights, boardInfo, !color); // if we captured, we might have messed up our opponent's castling rights
    }

    Piece_t type = PieceOnSquare(boardInfo, fromSquare);
    switch (type) {
        case pawn:
            nextState->halfmoveClock = 0;

            if(PawnIsDoublePushed(fromBB, toBB)) {
                nextState->enPassantSquares = GetEnPassantBB(toBB, color);
            }
        break;
        case king:
            ResetAllRights(&nextState->castleRights, color);
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

    UpdatecastleRights(&nextState->castleRights, boardInfo, color);
}

void MakeMove(BoardInfo_t* boardInfo, GameStack_t* gameStack, Move_t move, Color_t moveColor) {
    SpecialFlag_t specialFlag = ReadSpecialFlag(move);
    GameState_t* nextState = GetDefaultNextGameState(gameStack);
    
    switch (specialFlag) {
        case castle_flag:
            MakeCastlingHandler(boardInfo, nextState, move, moveColor);
        break;
        case promotion_flag:
            MakePromotionHandler(boardInfo, nextState, move, moveColor);
        break;
        case en_passant_flag:
            MakeEnPassantHandler(boardInfo, nextState, move, moveColor);
        break;
        default:
            MakeMoveDefaultHandler(boardInfo, nextState, move, moveColor);
        break;
    }

    UpdateEmpty(boardInfo);
    nextState->boardInfo = *boardInfo;
}

void UnmakeMove(BoardInfo_t* boardInfo, GameStack_t* gameStack) {
    RevertState(gameStack);
    *boardInfo = ReadCurrentBoardInfo(gameStack);
}