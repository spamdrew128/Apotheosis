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
    Color_t color
) 
{
    ResetBits(infoField, fromBB);
    SetBits(infoField, toBB);

    ResetBits(&(boardInfo->allPieces[color]), fromBB);
    SetBits(&(boardInfo->allPieces[color]), toBB);

    UpdateEmpty(boardInfo);
}

static void MakeCastlingHandler(BoardInfo_t* boardInfo, Move_t move, Color_t color) {
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);

    Bitboard_t kingBB = boardInfo->kings[color];

    if(toSquare < fromSquare) { // queenside castle
        boardInfo->kings[color] = GenShiftWest(kingBB, 2);
        ResetBits(&(boardInfo->allPieces[color]), kingBB);
        
        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->rooks[color]),
            GenShiftWest(kingBB, 4),
            GenShiftWest(kingBB, 1),
            color
        );

    } else {
        boardInfo->kings[color] = GenShiftEast(kingBB, 2);
        ResetBits(&(boardInfo->allPieces[color]), kingBB);

        UpdateBoardInfoField(
            boardInfo,
            &(boardInfo->rooks[color]),
            GenShiftEast(kingBB, 3),
            GenShiftEast(kingBB, 1),
            color
        );
    }

    GameState_t nextState = GetDefaultNextGameState();
    nextState.castleSquares[color] = empty_set;
    AddState(nextState);
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