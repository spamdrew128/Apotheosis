#include "make_and_unmake.h"
#include "game_state.h"
#include "lookup.h"

static void RemoveCapturedPiece(
    BoardInfo_t* boardInfo,
    Bitboard_t* infoField,
    Square_t capturedSquare,
    Color_t color
) 
{
    Bitboard_t caputuredBB = GetSingleBitset(capturedSquare);
    ResetBits(infoField, caputuredBB);
    ResetBits(&(boardInfo->allPieces[color]), caputuredBB);
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

static void MakeCastlingHandler(BoardInfo_t* boardInfo, Move_t move, Color_t color) {
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);

    Bitboard_t fromBB = boardInfo->kings[color];

    if(toSquare < fromSquare) { // queenside castle
        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->kings[color]),
            fromBB,
            GenShiftWest(fromBB, 2),
            fromSquare,
            toSquare,
            color
        );

        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->rooks[color]),
            GenShiftWest(fromBB, 4),
            GenShiftWest(fromBB, 1),
            fromSquare,
            toSquare,
            color
        );

    } else {
        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->kings[color]),
            fromBB,
            GenShiftEast(fromBB, 2),
            fromSquare,
            toSquare,
            color
        );

        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->rooks[color]),
            GenShiftEast(fromBB, 3),
            GenShiftEast(fromBB, 1),
            fromSquare,
            toSquare,
            color
        );
    }

    GameState_t nextState = GetDefaultNextGameState();
    nextState.castleSquares[color] = empty_set;
    AddState(nextState);

    UpdateEmpty(boardInfo);
}

void MakeMove(BoardInfo_t* boardInfo, Move_t move, Color_t color) {
    SpecialFlag_t specialFlag = ReadSpecialFlag(move);
    
    switch (specialFlag) {
        case castle_flag:
            MakeCastlingHandler(boardInfo, move, color);
        break;
        case promotion_flag:
        /* code */
        break;
        case en_passant_flag:
        /* code */
        break;
        default:
        break;
    }
}